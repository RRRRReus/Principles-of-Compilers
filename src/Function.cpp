#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>
#include <set>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <stack>

extern FILE* yyout;

bool Function::isallocaOperand(Operand *op)
{
    return std::find(allocaOperands.begin(),allocaOperands.end(),op)!=allocaOperands.end();
}

void Function::renameBlocks(BasicBlock *bb)
{
    fprintf(stderr,"基本块%d开始重命名\n",bb->getNo());
        for(auto &op:allocaOperands)
        {
            if(!op->NameStack.empty())
                op->last_val=op->NameStack.back();
        }
        fprintf(stderr,"基本块%d开始重命名指令\n",bb->getNo());
        Instruction *next;
        for(auto i = bb->getHead()->getNext(); i != bb->getHead(); i = next)
        {
            next=i->getNext();
            if(i->isAlloca())
            {
                if(isallocaOperand(i->getDef()))
                {
                    i->save=false;
                }
                
            }
            else if(i->isStore()&&isallocaOperand(i->getDef()))
            {
                fprintf(stderr,"store改变基本块%d的操作数%s的值为%s\n",bb->getNo(),i->getDef()->toStr().c_str(),dynamic_cast<StoreInstruction*>(i)->getUse()[1]->getSymbolEntry()->toStr().c_str());
                i->getDef()->last_val=dynamic_cast<StoreInstruction*>(i)->getUse()[1]->getSymbolEntry();
                //i->getDef()->NameStack.push_back(this_val);
                
                i->save=false;
            }
            else if(i->isLoad()&&isallocaOperand(i->getUse()[0]))
            {
                fprintf(stderr,"发现load指令\n");
                fprintf(stderr,"基本块%d的操作数%s的值是%s\n",bb->getNo(),i->getUse()[0]->toStr().c_str(),i->getUse()[0]->getSymbolEntry()->toStr().c_str());
                if(i->getUse()[0]->last_val!=nullptr)
                {
                    fprintf(stderr,"load!!基本块%d的操作数%s重命名为%s\n",bb->getNo(),i->getDef()->toStr().c_str(),i->getUse()[0]->last_val->toStr().c_str());
                    //fprintf(stderr,"load!!基本块%d的操作数%s重命名为%s\n",bb->getNo(),i->getDef()->toStr().c_str(),i->getDef()->last_val->toStr().c_str());
                    i->getDef()->renameSymbolEntry(i->getUse()[0]->last_val);
                }

                i->save=false;
            }
            else if(i->isPhi())
            {
                for(auto &op:allocaOperands)
                {
                    for(auto &phi:op->phiInsts)
                    {
                        if(phi==i)
                        {
                            fprintf(stderr,"phi改变基本块%d的操作数%s的值为%s\n",bb->getNo(),i->getDef()->toStr().c_str(),i->getDef()->getSymbolEntry()->toStr().c_str());
                            op->last_val=i->getDef()->getSymbolEntry();
                            break;
                        }
                    }
                }
            }
        }
        for(auto &i:bb->getSucc())
        {
                for(auto j = i->getHead()->getNext(); j != i->getHead(); j = j->getNext())
                {
                    if(j->isPhi())
                    {
                    for(auto &op:allocaOperands)
                    {
                        for(auto &phi:op->phiInsts)
                        {
                            if(phi==j)
                            {
                                if(op->last_val!=nullptr)
                                {
                                dynamic_cast<PhiInstruction*>(j)->incoming.push_back(std::make_pair(new Operand(op->last_val),bb));
                                fprintf(stderr,"加列表！基本块%d的phi指令插入基本块%d的操作数%s\n",i->getNo(),bb->getNo(),op->last_val->toStr().c_str());
                                }
                                else{
                                dynamic_cast<PhiInstruction*>(j)->incoming.push_back(std::make_pair(new Operand(new ConstantSymbolEntry(op->getSymbolEntry()->getType(),0)),bb));
                                    fprintf(stderr,"一种危险的操作，强行赋值0\n");
                                }

                            }
                        }
                    }
                        
                    }
                }
        }

        
        for(auto &op:allocaOperands)
        {
            if(op->last_val!=nullptr)
                op->NameStack.push_back(op->last_val);
        }

        for(auto &i:bb->DOMsucc)
        {
            renameBlocks(i);
        }
        for(auto &op:allocaOperands)
        {
            if(!op->NameStack.empty())
                op->NameStack.pop_back();
        }


}


void Function::removeUnreachableBlocks()
{
    for(auto &bb:block_list)
    {
        bb->reachable=false;
    }
    fprintf(stderr,"开始删除不可达基本块函数\n");
    fprintf(stderr,"基本块数目%ld\n",block_list.size());
    fprintf(stderr,"开始遍历可达基本块\n");
    std::set<BasicBlock *> v;   //用于记录已经访问过的基本块
    std::list<BasicBlock *> q;  //用于广度优先搜索
    q.push_back(entry);//将入口基本块加入队列
    v.insert(entry);//将入口基本块加入已访问集合
    while (!q.empty())//广度优先搜索
    {
        auto bb = q.front();//取出队列的第一个元素
        q.pop_front();//删除队列的第一个元素
       

        bb->reachable=true;
       
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)//遍历基本块的后继
        {
            if (v.find(*succ) == v.end())//如果后继不在已访问集合中
            {
                v.insert(*succ);//将后继加入已访问集合
                q.push_back(*succ);//将后继加入队列
            }
        }
    }

    
    fprintf(stderr,"开始删除其他不可达基本块\n");
    //int lastno=-1;


    for (auto it = block_list.begin(); it != block_list.end(); ) {
        fprintf(stderr,"块%d是否可达%d\n",(*it)->getNo(),(*it)->reachable);
        BasicBlock* bb = *it;
        if (bb->reachable==false) { // 示例条件：删除编号为偶数的块
            delete bb;
            it = block_list.begin(); // 删除元素并更新迭代器
            
        } else {
            ++it; // 仅在不删除元素时递增迭代器
        }
    }


    fprintf(stderr,"删除不可达基本块函数结束\n");
    fprintf(stderr,"基本块数目%ld\n",block_list.size());

}

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

void Function::insertBlock(BasicBlock *bb)
{
    block_list.push_back(bb);
    fprintf(stderr,"在函数中插入基本块%d\n",bb->getNo());
    fprintf(stderr,"基本块数目%ld\n",block_list.size());
}

// remove the basicblock bb from its block_list.//从基本块列表中删除基本块bb
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}
void Function::optimize()
{
    fprintf(stderr, "函数%s优化\n", sym_ptr->toStr().c_str());
    fprintf(stderr,"入口基本块的后继数目%d\n",entry->getNumOfSucc());
    int cco=0;
    for (auto &bb : block_list)
    {
        cco++;
        bb->getNo();
    }
    fprintf(stderr,"??基本块数目%d\n",cco);
    fprintf(stderr,"??0基本块数目%ld\n",block_list.size());

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
    {
        bb->optimize();
    }

    removeUnreachableBlocks();
    for (auto &bb : block_list)
    {
        fprintf(stderr,"基本块%d的前驱有以下基本块\n",bb->getNo());
        for(auto &i:bb->getPred())
        {
            fprintf(stderr,"基本块%d\n",i->getNo());
        }
        fprintf(stderr,"基本块%d的后继有以下基本块\n",bb->getNo());
        for(auto &i:bb->getSucc())
        {
            fprintf(stderr,"基本块%d\n",i->getNo());
        }





    }

    if(1&&(exit->getNumOfPred()==1)&&(!dynamic_cast<FunctionType*>(sym_ptr->getType())->getRetType()->isVoid()))
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

for (auto i = entry->getHead()->getNext(); i != entry->getHead(); i = i->getNext())
    {
        if(i->isAlloca())
        {
            if(i->getDef()->getSymbolEntry()->getType()->isPtr())
            {
                Type *Innertype= dynamic_cast<PointerType*>(i->getDef()->getSymbolEntry()->getType())->getValueType();
                if(Innertype->isIntArray()||Innertype->isFloatArray())
                {
                    continue;
                }
                if(Innertype->isPtr())
                {
                    continue;
                }
            }


            
            allocaOperands.push_back(dynamic_cast<AllocaInstruction*>(i)->getDef());
            
        }
        else
        {
            break;
        }
    }
    bool dophi=1;
    fprintf(stderr,"剩余的块数目%ld\n",block_list.size());
    if(dophi&&allocaOperands.size()>0&&block_list.size()<1000)
    {
        buildDominanceTree();
        fprintf(stderr,"支配树构建完成\n");
        printDominanceTree(stderr);

        if(block_list.size()>0)
        {
            PHIoptimize();

        }
    }
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
            if(bb->getNumOfPred()==0){}
            else{
                new UncondBrInstruction(exit, bb);//插入无条件跳转指令
                bb->addSucc(exit);//将出口基本块加入基本块的后继
                exit->addPred(bb);//将基本块加入出口基本块的前驱
            }
        }
        if(bb->empty()&&bb->getNumOfPred()!=0)//如果基本块为空
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
        fprintf(stderr,"bb->getNo()是 %d，他的支配前驱有 ",bb->getNo());
        for (auto& predBB : bb->DOMpred) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");
    }

        // 计算支配树的后继节点
    for (auto& bb : block_list) {
        fprintf(stderr,"bb->getNo()是 %d，他的支配后继有 ",bb->getNo());
        for (auto& predBB : bb->DOMsucc) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");
        fprintf(stderr,"bb->getNo()是 %d，他的支配边界有 ",bb->getNo());
        for (auto& predBB : bb->DomFrontier) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");

    }

}

void Function::PHIoptimize()
{
    fprintf(stderr,"开始phi优化\n");

    std::vector<BasicBlock *> Worklist;
    
    std::unordered_map<Operand *,BasicBlock *> PHIinserted;
//PHI指令插入位置确定
    for (auto &allocaOperand : allocaOperands)
    {
        std::unordered_set<BasicBlock *> phivisitedbb; // 记录已访问节点
        Worklist.clear();
        for(Instruction *i:allocaOperand->getUse())
        {
                Worklist.push_back(i->getParent());
        }
        for(BasicBlock *bb:Worklist)
        {
            fprintf(stderr,"操作数%s的使用者是基本块%d\n",allocaOperand->toStr().c_str(),bb->getNo());  
        }
        
        while(1)
        {
            if(Worklist.empty())
            {
                break;
            }
            BasicBlock *bb=*(Worklist.begin());
            Worklist.erase(Worklist.begin());
            for(auto df:bb->DomFrontier)
            {
                if(phivisitedbb.find(df)==phivisitedbb.end())
                {
                    phivisitedbb.insert(df);
                    fprintf(stderr,"基本块%d插入操作数%s的phi指令\n",df->getNo(),allocaOperand->toStr().c_str());
                    if(allocaOperand->getSymbolEntry()->getType()->isPtr())
                    {
                        Type *Innertype= dynamic_cast<PointerType*>(allocaOperand->getSymbolEntry()->getType())->getValueType();
                        allocaOperand->getSymbolEntry()->setType(Innertype);
                    }
                    Operand *phiOperand=new Operand(new TemporarySymbolEntry(allocaOperand->getSymbolEntry()->getType(),SymbolTable::getLabel()));
                    PhiInstruction *phi=new PhiInstruction(phiOperand);
                    allocaOperand->phiInsts.push_back(phi);

                    phi->save=true;
                    df->insertFront(phi);
                    if(std::find(Worklist.begin(),Worklist.end(),df)==Worklist.end())
                    {
                        Worklist.push_back(df);
                    }
                }
            }
        }

    }
    //重命名
    renameBlocks(entry);

    //刷新删除指令
    for(auto &bb:block_list)
    {
        bb->refresh();
        //bb->output();
        //fprintf(yyout,"----------------------\n");
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
    
    fprintf(stderr,"支配前驱集合计算完成\n");
    
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
    
    fprintf(stderr,"支配前驱集合排序完成\n");
    printDominanceTree(stderr);
    //计算支配树的后继节点
    for (auto& bb : block_list) {
        fprintf(stderr,"计算基本块%d的支配后继集合\n",bb->getNo());
        BasicBlock* parent;
        if(bb->DOMpred.size()<=1)
        {
            parent = nullptr;
        }
        else
        {
            parent = bb->DOMpred[1];
        }
        if (parent != nullptr) {
        fprintf(stderr,"1\n");
        fprintf(stderr,"parent->no = %d\n",parent->getNo());
        fprintf(stderr,"DOMsucc.size() = %ld\n",parent->DOMsucc.size());
            parent->DOMsucc.push_back(bb);
        fprintf(stderr,"1=2\n");

        }

    }
    fprintf(stderr,"支配后继集合计算完成\n");
    // 计算支配边界
        for (auto& bb : block_list) {


                std::queue<BasicBlock*> q;  // 用队列进行层序遍历
                q.push(bb);  // 将根节点加入队列

                while (!q.empty()) {
                    int level_size = q.size();  // 当前层的节点数


                    // 遍历当前层的所有节点
                    for (int i = 0; i < level_size; ++i) {
                        BasicBlock* node = q.front();
                        q.pop();
                        for(auto &j:node->getSucc())
                        {
                            if(j==bb||std::find(j->DOMpred.begin(),j->DOMpred.end(),bb)==j->DOMpred.end())
                            {
                                bb->DomFrontier.push_back(j);
                            }
                        }
                      
                        // 将当前节点的所有子节点加入队列
                        for (auto& child : node->DOMsucc) {
                            q.push(child);
                        }
                    }

                    








            }



}
}