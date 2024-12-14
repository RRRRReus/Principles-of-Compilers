#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include "Instruction.h"
#include <list>
#include <unordered_map>
#include <unordered_set>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);//插入unit的函数列表
    entry = new BasicBlock(this);//创建一个新的基本块
    exit = new BasicBlock(this);//创建一个新的基本块
    sym_ptr = s;//符号表项
    parent = u;
    //fprintf(stderr, "sym_ptr->getType()->getRetType()->toStr() = %s\n",dynamic_cast<FunctionType*>(sym_ptr->getType())->getRetType()->toStr().c_str());   
    Type *retType = dynamic_cast<FunctionType*>(sym_ptr->getType())->getRetType();//获取函数的返回值类型
    
    if(retType->isVoid())
    {
        return_val = nullptr;
        new RetInstruction(nullptr, exit);// 返回
    }
    else{
    Type *PointRetType = new PointerType(retType);//返回值类型的指针类型
    SymbolEntry *ret = new TemporarySymbolEntry(retType,SymbolTable::getLabel());//创建一个新的临时符号表项
    this->return_val = new Operand(new TemporarySymbolEntry(PointRetType,SymbolTable::getLabel()));//返回值操作数
    //入口基本块存放返回值的内存空间
    AllocaInstruction *alloc= new AllocaInstruction(return_val, ret);// 分配返回值的内存空间
    entry->insertFront(alloc);//将指令插入到基本块的最前面
    new StoreInstruction(return_val, new Operand(new ConstantSymbolEntry(retType, 0)), entry);// 初始化返回值为0

    //出口基本块加载返回值
    Operand *load = new Operand(new TemporarySymbolEntry(retType, SymbolTable::getLabel()));//创建一个新的临时符号表项
    new LoadInstruction(load, return_val, exit);// 加载返回值
    new RetInstruction(load, exit);// 返回
    }
}
Function::~Function()
{
    auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.//从基本块列表中删除基本块bb
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}
void Function::optimize()
{
    fprintf(stderr, "函数%s优化\n", sym_ptr->toStr().c_str());
    for (auto &bb : block_list)
    {
        bb->cleanPred();
        bb->cleanSucc();
        if(!(bb->rbegin()->isCond()||bb->rbegin()->isUncond()||bb==exit)||bb->empty())
        {
            new UncondBrInstruction(exit, bb);//插入无条件跳转指令
        }

    }

    for (auto &bb : block_list)
        bb->optimize();


    if((exit->getNumOfPred()==1)&&(!dynamic_cast<FunctionType*>(sym_ptr->getType())->getRetType()->isVoid()))
    {
        fprintf(stderr,"单一出口鱼贯合并优化");
        BasicBlock *bb=*(exit->pred_begin());
        fprintf(stderr,"qqq这个基本块是%d\n",bb->getNo());
        bb->removeSucc(exit);
        exit->removePred(bb);
        Instruction *inst=bb->rbegin();
        Instruction *inst2=inst->getPrev();

        Operand *op=dynamic_cast<StoreInstruction*>(inst2)->getUse()[1];
        if(bb!=entry)
            bb->remove(inst2);
        bb->remove(inst);
        //new RetInstruction(op, bb);
        RetInstruction*rel= new RetInstruction(op, bb);
        rel->save=true;

        for(auto &i:return_val->getUse())
        {
            if(i->getParent()==entry)
            {
                entry->remove(i);
            }
        }

        entry->remove(return_val->getDef());
        

    }

    deadCodeElimination();//执行死代码消除优化
    //aggressiveDeadCodeElimination();

}
void Function::deadCodeElimination()
{
    fprintf(stderr, "开始执行函数%s死代码消除\n", sym_ptr->toStr().c_str());
    std::unordered_map<Operand*, Instruction*> defMap;//存储所有操作数的定义指令
    std::unordered_map<Operand*, std::unordered_set<Instruction*>> useMap;//来记录所有 <变量,使用它的所有指令>
    std::unordered_set<Operand*> workList; //存储所有需要处理的操作数
    std::unordered_set<Operand*> functionParams(params.begin(), params.end());  //当前函数的参数？？？而不是调用函数的参数？？？

    // 初始化 defMap 和 useMap
    for (BasicBlock* bb : block_list)//遍历所有基本块
    {
        for (Instruction* inst = bb->begin(); inst != bb->end(); inst = inst->getNext())//遍历当前块中的所有指令
        {
            Operand* def = inst->getDef();//获取当前指令的def
            if (def!=nullptr)//如果def不为空
            {
                defMap[def] = inst;
                workList.insert(def);   
                std::vector<Instruction*> useInst = def->getUse();//获取所有use了当前操作数的指令
                useMap[def] = std::unordered_set<Instruction*>(useInst.begin(), useInst.end());//将useMap中的useInst加入到useMap中
            }

        }
    }

    // 处理工作列表
    while (!workList.empty())
    {
        Operand* v = *workList.begin();//获取workList的第一个operand
        workList.erase(workList.begin());//删除workList的第一个元素,迭代

        fprintf(stderr, "当前处理的操作数是%s\n", v->toStr().c_str());
        fprintf(stderr, "当前操作数的use数量:%ld\n", useMap[v].size());

        if(useMap[v].empty())//如果v的使用列表为空
        {
            fprintf(stderr,"当前v是%s\n",v->toStr().c_str());
            Instruction* defInst = nullptr;//v若为常数，则无定义语句

            if(defMap[v]!=nullptr){
                defInst = defMap[v];//获取v的定义指令
            }
    
            if(defInst!=nullptr && !defInst->hasSideEffects())//如果def没有副作用
            {

                defInst->save = false;//将def的save设置为false
                //defInst->output();

                fprintf(stderr, "删除指令的def是%s\n", defInst->getDef()->toStr().c_str());
                fprintf(stderr, "删除指令的类型为%d\n", defInst->getInstType());
                for(Operand* u : defInst->getUse())//遍历defInst的所有use   //u此时是v的定义语句的其中一个use
                {
                    useMap[u].erase(defInst);//将def从useMap中的u的use中删除
                    //将变量u加入到workList中
                    if(functionParams.find(u) == functionParams.end())//函数的入参并不在我们的考量范围内（我们总不能消掉它们的def吧）
                    {
                        workList.insert(u);//维持def-use链，之前worklist里只有def，当前def删掉后，当前指令的use加入worklist
                    }
                }
            }

        }
    }

    for(auto &bb:block_list)
    {
        bb->refresh();
    }

    fprintf(stderr, "函数%s死代码消除结束\n", sym_ptr->toStr().c_str());
}

void Function::aggressiveDeadCodeElimination()
{
    // fprintf(stderr, "开始执行函数%s激进死代码消除\n", sym_ptr->toStr().c_str());
    // std::unordered_set<Instruction*> live;  //存储所有活跃的指令
    // std::unordered_set<BasicBlock*> liveBlock;  //存储所有活跃的基本块
    // std::unordered_set<Operand*> liveUse;   //存储所有活跃的use
    // std::unordered_set<Instruction*> workList;  //存储所有需要处理的指令
    // std::unordered_map<Operand*, Instruction*> defMap;  //存储所有操作数的定义指令

    // // 初始化 defMap 和 workList
    // for (BasicBlock* bb : block_list) {
    //     for (Instruction* inst = bb->begin(); inst != bb->end(); inst = inst->getNext()) 
    //     {
    //         Operand* def = inst->getDef();//获取当前指令的def
    //         if (def) 
    //         {
    //             defMap[def] = inst;
    //         }
    //         if (inst->hasSideEffects()) 
    //         {
    //             workList.insert(inst);//将有副作用的指令加入到workList中
    //         }
    //     }
    // }

    // // 处理工作列表
    // while (!workList.empty()) 
    // {
    //     Instruction* inst = *workList.begin();
    //     workList.erase(workList.begin());
    //     live.insert(inst);
    //     liveBlock.insert(inst->getParent());
    //     for (Operand* use : inst->getUse()) {
    //         liveUse.insert(use);
    //     }

    //     // 对于 phi 指令，标记其前驱块的终结指令为活跃
    //     if (auto* phiInst = dynamic_cast<PhiInstruction*>(inst)) {
    //         for (auto& [block, operand] : phiInst->getBlockMap()) {
    //             if (block->getTerminal() && live.find(block->getTerminal()) == live.end()) {
    //                 workList.insert(block->getTerminal());
    //                 liveBlock.insert(block);
    //             }
    //         }
    //     }

    //     // 加入该块的所有控制依赖前驱
    //     for (BasicBlock* cdg_pred : inst->getParent()->getControlDependencePredecessors()) {
    //         if (cdg_pred->getTerminal() && live.find(cdg_pred->getTerminal()) == live.end()) {
    //             workList.insert(cdg_pred->getTerminal());
    //         }
    //     }

    //     // 对于每个 use 的变量，将其 def 加入 workList
    //     for (Operand* use : inst->getUse()) {
    //         if (auto* reg = dynamic_cast<IRRegister*>(use)) {
    //             Instruction* def = defMap[use];
    //             if (def && live.find(def) == live.end()) {
    //                 workList.insert(def);
    //             }
    //         }
    //     }
    // }


    // // 遍历所有指令，删除不活跃的指令
    // for (BasicBlock* bb : block_list) {
    //     for (Instruction* inst = bb->getFirstInstruction(); inst != nullptr; ) {
    //         Instruction* nextInst = inst->getNext();
    //         if (live.find(inst) == live.end()) {
    //             Operand* def = inst->getDef();
    //             if (def) {
    //                 defMap.erase(def);
    //             }
    //             for (Operand* use : inst->getUse()) {
    //                 liveUse.erase(use);
    //             }
    //             inst->remove();
    //         }
    //         inst = nextInst;
    //     }
    // }


}

void Function::output() const
{
    // if(this->block_list.size() == 2)
    // {
    //     new UncondBrInstruction(exit, entry);//插入无条件跳转指令
    //     entry->addSucc(exit);//将出口基本块加入基本块的后继
    //     exit->addPred(entry);//将基本块加入出口基本块的前驱
    // }
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s(", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    for(long unsigned int i = 0; i < params.size(); i++)
    {
        fprintf(stderr,"params[%ld] = %s\n",i,params[i]->toStr().c_str());
        fprintf(yyout, "%s %s", params[i]->getType()->toStr().c_str(), params[i]->toStr().c_str());
        if(i != params.size() - 1)
            fprintf(yyout, ", ");
    }
    fprintf(yyout, ") {\n");
    fprintf(stderr,"已输出define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    std::set<BasicBlock *> v;   //用于记录已经访问过的基本块
    std::list<BasicBlock *> q;  //用于广度优先搜索
    q.push_back(entry);//将入口基本块加入队列
    v.insert(entry);//将入口基本块加入已访问集合
    while (!q.empty())//广度优先搜索
    {
        auto bb = q.front();//取出队列的第一个元素
        q.pop_front();//删除队列的第一个元素


        if(!(bb->rbegin()->isCond()||bb->rbegin()->isUncond()||bb==exit||bb->rbegin()->isRet()))
        {
            new UncondBrInstruction(exit, bb);//插入无条件跳转指令
            bb->addSucc(exit);//将出口基本块加入基本块的后继
            exit->addPred(bb);//将基本块加入出口基本块的前驱
        }
        if(bb->empty())//如果基本块为空
        {
            new UncondBrInstruction(exit, bb);//插入无条件跳转指令
            bb->addSucc(exit);//将出口基本块加入基本块的后继
            exit->addPred(bb);//将基本块加入出口基本块的前驱
        }
        bb->output();//输出基本块
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)//遍历基本块的后继
        {
            if (v.find(*succ) == v.end())//如果后继不在已访问集合中
            {
                v.insert(*succ);//将后继加入已访问集合
                q.push_back(*succ);//将后继加入队列
            }
        }
    }
    fprintf(yyout, "}\n");
}


