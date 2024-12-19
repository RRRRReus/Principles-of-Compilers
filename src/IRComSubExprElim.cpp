#include "IRComSubExprElim.h"
#include <queue>

IRComSubExprElim::IRComSubExprElim(Unit *unit)
{
    this->unit = unit;
}

IRComSubExprElim::~IRComSubExprElim()
{
}

bool IRComSubExprElim::skip(Instruction *inst)
{
    /**
     * 判断当前指令是否可以当成一个表达式
     * 当前只将二元运算指令当作表达式
     * 纯函数及一些一元指令也可当作表达式
     */
    if (dynamic_cast<BinaryInstruction *>(inst) != nullptr)
        return false;
    return true;
}

bool IRComSubExprElim::localCSE(Function *func)
{
    fprintf(stderr, "局部公共子表达式消除\n");



    Expr e1 = Expr(func->getEntry()->begin());

    Expr e2 = Expr(func->getEntry()->begin());
    
    fprintf(stderr, "是不是相同的表达式%d\n", e1 == e2);
    bool result = true;//？？应该迭代？？
    std::vector<Expr> exprs;
    for (auto block = func->begin(); block != func->end(); block++)//???应该支配树
    {
        exprs.clear();
        for (auto inst = (*block)->begin(); inst != (*block)->end(); inst = inst->getNext())
        {
            if (skip(inst))
                continue;
            auto preInstIt = std::find(exprs.begin(), exprs.end(), Expr(inst));
            if (preInstIt != exprs.end())
            {


                fprintf(stderr, "inst %d 是公共子表达式\n", inst->getInstType());
                fprintf(stderr, "删掉inst->getDef() %s\n", inst->getDef()->toStr().c_str());
                fprintf(stderr, "她她她换成preInstIt->inst->getDef() %s\n", preInstIt->inst->getDef()->toStr().c_str());
                // TODO: 把对当前指令的def的use改成对于preInst的def的use，并删除当前指令。
                inst->getDef()->replaceAllUsesWith(preInstIt->inst->getDef());
                inst->save=false;
                result = false;
            }
            else
                exprs.emplace_back(inst);
            /**
             * 这里不需要考虑表达式注销的问题
             * 因为ir是ssa形式的代码，目前来说应该不会有这样的情况，这种是错的
             * a = b + c
             * b = d + f
             */
            
        }
        (*block)->refresh();
    }

    //result = true;
    return result;
}

bool IRComSubExprElim::globalCSE(Function *func)
{
    fprintf(stderr, "全局公共子表达式消除\n");
    exprVec.clear();
    ins2Expr.clear();
    genBB.clear();
    killBB.clear();
    inBB.clear();
    outBB.clear();

    bool result = true; 
    calGenKill(func);
    calInOut(func);
    result = removeGlobalCSE(func);
    return result;
}
/**
 * 计算gen kill in out
 * 1. 计算gen
 * 2. 计算kill
 * 3. 计算in out
 * 4. 全局公共子表达式消除
 * 
 */
void IRComSubExprElim::calGenKill(Function *func)
{
    // 计算gen
    for (auto block = func->begin(); block != func->end(); block++)
    {
        for (auto inst = (*block)->begin(); inst != (*block)->end(); inst = inst->getNext())
        {
            if (skip(inst))
                continue;
            Expr expr(inst);
            // 对于表达式a + b，我们只需要全局记录一次，重复出现的话，用同一个id即可
            auto it = find(exprVec.begin(), exprVec.end(), expr);
            int ind = it - exprVec.begin();
            if (it == exprVec.end())
            {
                exprVec.push_back(expr);
            }
            ins2Expr[inst] = ind;
            genBB[*block].insert(ind);
            /*
                一个基本块内不会出现这种 t1 = t2 + t3
                                       t2 = ...
                所以这里，之后gen的表达式不会kill掉已经gen的表达式
                就算是phi指令，也是并行取值，所以问题不大哦
            */
        }
    }
    // 计算kill
    for (auto block = func->begin(); block != func->end(); block++)
    {
        for (auto inst = (*block)->begin(); inst != (*block)->end(); inst = inst->getNext())
        {
            if (inst->getDef() != nullptr)
            {
                for (auto useInst : inst->getDef()->getUse())
                {
                    if (!skip(useInst))
                        killBB[*block].insert(ins2Expr[useInst]);
                }
            }
        }
    }
}

void IRComSubExprElim::calInOut(Function *func)
{
    std::set<int> U;
    for (size_t i = 0; i < exprVec.size(); i++)
        U.insert(i);
    auto entry = func->getEntry();
    inBB[entry].clear();
    outBB[entry] = genBB[entry];
    // 初始化除entry外的基本块的out为U
    std::set<BasicBlock *> workList;
    for (auto block = func->begin(); block != func->end(); block++)
    {
        if (*block != entry) {
            outBB[*block] = U;
            workList.insert(*block);
        }
    }
    // 不断迭代直到收敛
    while (!workList.empty())
    {
        auto block = *workList.begin();
        workList.erase(workList.begin());
        // 计算in[block] = U outBB[predBB];
        std::set<int> in[2];
        if (block->getNumOfPred() > 0)
            in[0] = outBB[*block->pred_begin()];
        auto it = block->pred_begin();
        it++;
        int turn = 1;
        for (; it != block->pred_end(); it++)
        {
            in[turn].clear();
            std::set_intersection(outBB[*it].begin(), outBB[*it].end(), in[turn ^ 1].begin(), in[turn ^ 1].end(), inserter(in[turn], in[turn].begin()));
            turn ^= 1;
        }
        inBB[block] = in[turn ^ 1];
        // 计算outBB[block] = (inBB[block] - killBB[block]) U genBB[block];
        std::set<int> midDif;
        std::set<int> out;
        std::set_difference(inBB[block].begin(), inBB[block].end(), killBB[block].begin(), killBB[block].end(), inserter(midDif, midDif.begin()));
        std::set_union(genBB[block].begin(), genBB[block].end(), midDif.begin(), midDif.end(), inserter(out, out.begin()));
        if (out != outBB[block])
        {
            outBB[block] = out;
            for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
                workList.insert(*succ);
        }
    }
}

bool IRComSubExprElim::removeGlobalCSE(Function *func) {
    fprintf(stderr, "全局公共子表达式消除\n");

    bool changed = true;  // 标记是否发生了优化
    
    // 遍历函数中的每个基本块
    for (auto block = func->begin(); block != func->end(); ++block) {
        // 遍历基本块中的每个指令
        for (auto inst = (*block)->begin(); inst != (*block)->end(); inst = inst->getNext()) {
            if (skip(inst)) {
                continue;
            }

            Expr expr(inst);  // 当前指令表示的表达式
            auto it = find(exprVec.begin(), exprVec.end(), expr);

            // 如果找到了一个相同的表达式，说明这个表达式已经计算过
            if (it != exprVec.end()) {
                int exprID = it - exprVec.begin();  // 找到的表达式的ID
                int existingExprID = ins2Expr[inst];  // 当前指令对应的表达式ID

                // 如果表达式ID不一致，说明是一个全新的表达式
                if (existingExprID != exprID) {
                    fprintf(stderr, "发现公共子表达式: %s, 替换指令: %s\n", expr.inst->getDef()->toStr().c_str(), inst->getDef()->toStr().c_str());

                    // 替换当前指令的定义为已存在的表达式的定义
                    inst->getDef()->replaceAllUsesWith(exprVec[exprID].inst->getDef());
                    inst->save = false;  // 删除当前指令
                    changed = true;
                }
            } else {
                // 如果表达式未出现过，加入exprVec并更新相关信息
                exprVec.push_back(expr);
                ins2Expr[inst] = exprVec.size() - 1;  // 更新该指令对应的表达式ID
                genBB[*block].insert(ins2Expr[inst]);  // 更新gen集合
            }
        }

        // 更新基本块
        (*block)->refresh();
    }

    // 全局消除过程中，检查是否可以使用跨基本块的信息
    for (auto block = func->begin(); block != func->end(); ++block) {
        std::set<int> currentIn = inBB[*block];  // 当前基本块的in集合
        std::set<int> currentOut = outBB[*block];  // 当前基本块的out集合

        // 对于每个基本块，检查它的出边
        for (auto succ = (*block)->succ_begin(); succ != (*block)->succ_end(); ++succ) {
            // 对于每个后继基本块，更新当前基本块的out集合
            std::set<int> tempOut;
            std::set_union(currentOut.begin(), currentOut.end(),
                           outBB[*succ].begin(), outBB[*succ].end(),
                           std::inserter(tempOut, tempOut.begin()));

            // 如果当前基本块的out集合与先前的out集合不一样，标记发生变化
            if (tempOut != outBB[*block]) {
                outBB[*block] = tempOut;
                changed = false;
            }
        }
    }
    changed = true;
    return changed;  // 返回是否发生了变化
}

void IRComSubExprElim::pass()
{
    fprintf(stderr, "全局公共子表达式消除pass\n");
    for (auto func = unit->begin(); func != unit->end(); func++)
    {
        while (!localCSE(*func) || !globalCSE(*func))
            ;
    }
}
