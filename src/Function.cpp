#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>
#include <set>
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
        new RetInstruction(op, bb);

        for(auto &i:return_val->getUse())
        {
            if(i->getParent()==entry)
            {
                entry->remove(i);
            }
        }

        entry->remove(return_val->getDef());
        

    }


    buildDominanceTree();
    printDominanceTree(stderr);
    //fprintf(stderr, "函数%s优化完成\n", sym_ptr->toStr().c_str());
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
// 输出支配树的递归函数

// 输出支配树
void Function::printDominanceTree(FILE* out) {
        for (auto& bb : block_list) {
        fprintf(stderr,"bb->getNo()是 %d，他的前驱有 ",bb->getNo());
        for (auto& predBB : bb->DOMpred) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");
    }

        // 计算支配树的后继节点
    for (auto& bb : block_list) {
        fprintf(stderr,"bb->getNo()是 %d，他的后继有 ",bb->getNo());
        for (auto& predBB : bb->DOMsucc) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");
    }

}

// 构建支配树的算法
void Function::buildDominanceTree() {
    if (block_list.empty()) {
        return;
    }

    // 初始化支配树根节点为入口基本块
    DomTreeRoot = entry;

    // 使用一个集合来计算每个基本块的支配集合
    for (auto& bb : block_list) {
        // 初始化每个基本块的支配前驱为空
        bb->setDOMpred({});
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto& bb : block_list) {
            fprintf(stderr,"bb->getNo() = %d\n",bb->getNo());
            std::vector<BasicBlock*> newDOMpred;
            if (bb == entry) {
                // 入口基本块的支配前驱是它自己
                newDOMpred.push_back(bb);
            } else {
                // 对于每个基本块，计算其支配集合
                bool first = true;
                for (auto& predBB : bb->getPred()) {
                    if (first) {
                        for (auto& domPred : predBB->DOMpred) {
                                newDOMpred.push_back(domPred);
                        }
                        first = false;
                    } else {
                        // 取前驱的交集
                        std::vector<BasicBlock*> intersection;
                        for (auto& domPred : predBB->DOMpred) {
                            if (std::find(newDOMpred.begin(), newDOMpred.end(), domPred) != newDOMpred.end()) {
                                intersection.push_back(domPred);
                            }
                        }
                        newDOMpred = intersection;
                        
                    }
                

                    // fprintf(stderr,"predBB->getNo() = %d\n",predBB->getNo());
                    // fprintf(stderr,"predBB->DOMpred.size() = %ld\n",predBB->DOMpred.size());
                    // fprintf(stderr,"newDOMpred.size() = %ld\n",newDOMpred.size());
                }
                if(std::find(newDOMpred.begin(), newDOMpred.end(), bb) == newDOMpred.end())
                    newDOMpred.push_back(bb);
                if(std::find(newDOMpred.begin(), newDOMpred.end(), entry) == newDOMpred.end())
                    newDOMpred.push_back(entry);


            }

            // 检查是否需要更新支配前驱集合
            if (newDOMpred != bb->DOMpred) {
                fprintf(stderr,"!!!bb->getNo() = %d\n",bb->getNo());
                bb->setDOMpred(newDOMpred);
                changed = true;
            }
        }
    }
    //支配树的前驱排序
    for(auto &bb:block_list)
    {
        int len=int(bb->DOMpred.size());
        for(int i=0;i<len;i++)
        {
            for(int j=i+1;j<len;j++)
            {
                if(std::find(bb->DOMpred[i]->DOMpred.begin(),bb->DOMpred[i]->DOMpred.end(),bb->DOMpred[j])==bb->DOMpred[i]->DOMpred.end())
                {
                    BasicBlock *temp=bb->DOMpred[i];
                    bb->DOMpred[i]=bb->DOMpred[j];
                    bb->DOMpred[j]=temp;
                }
            }
        }
    }
    //计算支配树的后继节点
    for (auto& bb : block_list) {
        BasicBlock* parent = bb->DOMpred.empty() ? nullptr : bb->DOMpred[1];
        if (parent != nullptr) {
            parent->DOMsucc.push_back(bb);
        }
    }


}