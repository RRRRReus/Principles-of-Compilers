#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include "Instruction.h"
#include <list>
#include <set>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "AsmBuilder.h"
#include "MachineCode.h"

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
                i->getDef()->last_val=dynamic_cast<StoreInstruction*>(i)->getUse()[1];
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
                    //i->getDef()->renameSymbolEntry(i->getUse()[0]->last_val);
                    i->getDef()->replaceAllUsesWith(i->getUse()[0]->last_val);
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
                            op->last_val=i->getDef();
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
                                //dynamic_cast<PhiInstruction*>(j)->incoming.push_back(std::make_pair(new Operand(op->last_val),bb));
                                
                                dynamic_cast<PhiInstruction*>(j)->addIncoming(op->last_val,bb);
                                fprintf(stderr,"加列表！基本块%d的phi指令插入基本块%d的操作数%s\n",i->getNo(),bb->getNo(),op->last_val->toStr().c_str());
                                }
                                else{
                                //dynamic_cast<PhiInstruction*>(j)->incoming.push_back(std::make_pair(new Operand(new ConstantSymbolEntry(op->getSymbolEntry()->getType(),0)),bb));
                                dynamic_cast<PhiInstruction*>(j)->addIncoming(new Operand(new ConstantSymbolEntry(op->getSymbolEntry()->getType(),0)),bb);

                                    
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
        if (bb->reachable==false) { 
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
       

        block_list.erase(std::find(block_list.begin(), block_list.end(), exit));
        exit=bb;
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
            //PHIoptimize();
        }
        
    }
    sccp();
    //fprintf(stderr, "函数%s优化完成\n", sym_ptr->toStr().c_str());
    //deadCodeElimination();//执行死代码消除优化
    if(block_list.size()<1000)
    {
        //aggressiveDeadCodeElimination();
    }
    

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

    fprintf(stderr, "开始执行函数%s激进死代码消除\n", sym_ptr->toStr().c_str());
    std::unordered_set<Instruction*> live;  //存储所有活跃的指令
    std::unordered_set<BasicBlock*> liveBlock;  //存储所有活跃的基本块
    std::unordered_set<Operand*> liveUse;   //存储所有活跃的use
    std::unordered_set<Instruction*> workList;  //存储所有需要处理的指令
    std::unordered_map<Operand*, Instruction*> defMap;  //存储所有操作数的定义指令
    std::unordered_set<Operand*> functionParams(params.begin(), params.end());
    

    // 初始化 defMap 和 workList
    for (BasicBlock* bb : block_list) {
        for (Instruction* inst = bb->begin(); inst != bb->end(); inst = inst->getNext()) 
        {
            fprintf(stderr, "当前基本块为%d\n", bb->getNo());
            fprintf(stderr, "当前基本块的全部指令%d\n", inst->getInstType());
            Operand* def = inst->getDef();//获取当前指令的def
            if (def!=nullptr&&!inst->isStore()) 
            {
                defMap[def] = inst;
            }
            if (inst->isCall() || inst->isRet() || inst->isStore()) //有副作用的指令，即store全局变量的指令、Ret指令、Call指令
            {
                if(inst->isStore())//store指令需要判断一下是不是全局的
                {
                //     fprintf(stderr, "这句话可能输出！！！\n");
                //     fprintf(stderr, "inst->getDef()->getSymbolEntry()->toStr() = %s\n", inst->getDef()->getSymbolEntry()->toStr().c_str());
                //     fprintf(stderr, "inst->getDef()->getSymbolEntry()->toStr() = %s\n", inst->getDef()->getSymbolEntry()->toStr().c_str());
                //     fprintf(stderr, "inst->getDef()->getSymbolEntry()->isVariable() = %d\n", inst->getDef()->getSymbolEntry()->isVariable());
                    
                    
                    

                //    //如果是对全局变量进行store
                //     if(inst->getDef()->getSymbolEntry()->isVariable())
                //     {
                //         if(dynamic_cast<IdentifierSymbolEntry*>(inst->getDef()->getSymbolEntry())->isGlobal())
                //         {
                //             fprintf(stderr, "这句话不可能输出！！！\n");
                //             workList.insert(inst);

                //         }
                //     }
                //     else{
                //         fprintf(stderr, "这句话bshi！！！\n");
                //         fprintf(stderr, "inst->getDef():%s\n", inst->getDef()->toStr().c_str());
                //         fprintf(stderr, "inst->getDef()->getDef()->type:%d\n", inst->getDef()->getDef()->getInstType());
                //         //fprintf(stderr, "inst->getDef()->getDef()->getUse()[0]:%s\n", inst->getDef()->getDef()->getUse()[0]->toStr().c_str());
                //         Operand* base=nullptr;

                //         if(inst->getDef()->getDef()->isGep())
                //             base=inst->getDef()->getDef()->getUse()[0];
                        
                //         if(base!=nullptr)
                //         {
                //             fprintf(stderr, "666base:%s\n", base->toStr().c_str());
                //         }
                //         fprintf(stderr, "这句话2222bshi！！！\n");
                //         if(base&&base->getSymbolEntry()->isVariable())
                //         {
                //             fprintf(stderr, "base:%s\n", base->toStr().c_str());
                //             if(dynamic_cast<IdentifierSymbolEntry*>(base->getSymbolEntry())->isGlobal())
                //             {
                //                 fprintf(stderr, "\n");
                //                 workList.insert(inst);
                //             }

                //         }
                //         if(functionParams.find(base) != functionParams.end())
                //             {
                //                 fprintf(stderr, "这句话不可rrr能输出！！！\n");
                //                 workList.insert(inst);
                //             }
                //     }

                  workList.insert(inst);  //这个地方有问题！！！！！！！将来看到记得考虑



                    // // 如果是数组类型，直接将指令视为活跃
                    // else if(inst->getDef()->getSymbolEntry()->getType()->isAllArray())
                    // {
                    //     fprintf(stderr, "遇到数组类型，直接将指令视为活跃\n");
                    //     live.insert(inst);
                    //     liveBlock.insert(inst->getParent());
                    //     for (Operand* use : inst->getUse()) {
                    //         liveUse.insert(use);
                    //     }
                    // }
                    
                      
                }
                
                else
                {
                    workList.insert(inst);//将有副作用的指令加入到workList中
                    fprintf(stderr, "有副作用的指令是%d\n", inst->getInstType());
                }
                
            }
        }
    }

    fprintf(stderr, "worklist、defMap初始化完成\n");

    //1、求反CFG
    std::vector<BasicBlock*> reverseCFG;
    createReverseCFG();

    //2、调用jj函数，传入反CFG的入口块，求出前向支配树的支配边界
    buildReverseDominanceTree(reverseEntry);
    fprintf(stderr,"反支配树构建完成！！！！！\n");
    printReverseDominanceTree(stderr);

    //3、根据这个支配边界，求出控制依赖前驱

    // 处理工作列表
    while (!workList.empty()) 
    {
        Instruction* inst = *workList.begin();
        fprintf(stderr, "当前处理的指令所属块是%d\n", inst->getParent()->getNo());
        workList.erase(workList.begin());
        fprintf(stderr, "当前处理的指令是%d\n", inst->getInstType());

        live.insert(inst);
        liveBlock.insert(inst->getParent());
        for (Operand* use : inst->getUse()) {
            liveUse.insert(use);
        }


        if(inst->isAlloca()||inst->isGep())
        {
            fprintf(stderr, "当前处理的指令是alloca\n");
            for(auto &i:inst->getDef()->getUse())
            {
                if(live.find(i)==live.end())
                workList.insert(i);
            }
            

        }

        
        // 对于 phi 指令，标记其前驱块的终结指令为活跃
        if (auto* phiInst = dynamic_cast<PhiInstruction*>(inst)) {
            fprintf(stderr, "当前处理的指令是phi\n");
            for (auto& [operand, block] : phiInst->incoming)//遍历phi指令所有前驱块
            {
                fprintf(stderr, "当前phi指令的前驱块是%d\n", block->getNo());
                if (block->getTerminal() && live.find(block->getTerminal()) == live.end()) //如果前驱块的终结指令不在live中
                {
                    workList.insert(block->getTerminal());//将前驱块的终结指令加入workList
                    fprintf(stderr, "当前被标记为活跃的块是%d\n", block->getNo());
                    liveBlock.insert(block);//标记前驱块为活跃
                }
            }
        }
        //若要求控制依赖前驱，则需有控制依赖图 / 前向支配树的支配边界

            //1、求反CFG
            //2、调用jj函数，传入反CFG的入口块，求出前向支配树的支配边界
            //3、根据这个支配边界，求出控制依赖前驱


        // 加入该块的所有控制依赖前驱
        fprintf(stderr, "当前块是%d\n", inst->getParent()->getNo());
        for (auto cdg_pred : inst->getParent()->reverseDomFrontier) { // 遍历支配边界
            if (cdg_pred->getTerminal() != nullptr && live.find(cdg_pred->getTerminal()) == live.end()) {
                workList.insert(cdg_pred->getTerminal()); // 注意已经加过的不用加了
    
                fprintf(stderr, "当前控制依赖前驱块是%d\n", cdg_pred->getNo());
            }
            fprintf(stderr, "所有控制依赖前驱块是%d\n", cdg_pred->getNo());
        }

        
        fprintf(stderr,"已遍历完该块所有的控制依赖前驱\n");


        // 对于每个 use 的变量，将其 def 加入 workList
        for (Operand* use : inst->getUse()) {
            // 检查 use 是否是 IRRegister 类型且不是 IRGlobalVar 类型 //是否是局部变量，而不是全局变量 //那函数参数呢？？？
            fprintf(stderr,"?????\n");
            fprintf(stderr,"use是%s\n",use->toStr().c_str());
            fprintf(stderr,"use的类型是%s\n",use->getSymbolEntry()->getType()->toStr().c_str());

            // if (!dynamic_cast<IdentifierSymbolEntry*>(use->getSymbolEntry())->isLocal()) {
            //             continue;
            //         }
            
            fprintf(stderr,"已continue！！！\n");
            Instruction* def = defMap[use];
            // if(use->getSymbolEntry()->isTemporary())
            // {
            //     fprintf(stderr,"def是%d\n",def->getInstType());
            //     fprintf(stderr,"def的def是%s\n",def->getDef()->toStr().c_str());
            // }
            
            if (def && live.find(def) == live.end()) {
                fprintf(stderr, "当前处理的指令的def是%d\n", def->getInstType());
                workList.insert(def);
            }


            if(use->getSymbolEntry()->getType()->isPtr()&&functionParams.find(use) != functionParams.end())
            {
                
                for(auto &i:use->getUse())
                {
                    if(live.find(i)==live.end())
                    workList.insert(i);
                }
            }

            if(use->getSymbolEntry()->isVariable()&&dynamic_cast<IdentifierSymbolEntry*>(use->getSymbolEntry())->isGlobal())
            {
                
                for(auto &i:use->getUse())
                {
                    if(live.find(i)==live.end())
                    workList.insert(i);
                }
            }

        }


    }

    fprintf(stderr, "worklist处理完成，活跃分析完成\n");

    for (Operand* use : liveUse) {
            fprintf(stderr, "当前活跃的use是%s\n", use->toStr().c_str());
        }


    // 遍历所有指令，删除不活跃的指令
    for (BasicBlock* bb : block_list) 
    {
        for (Instruction* inst = bb->begin(); inst != bb->end(); ) 
        {
            Instruction* nextInst = inst->getNext();
            if (live.find(inst) == live.end())//当前指令不在live中 
            {
                Operand* def = inst->getDef();
                if (def) 
                {
                    defMap.erase(def);
                }
                for (Operand* use : inst->getUse()) 
                {
                    liveUse.erase(use);
                }
                inst->save = false;
                //fprintf(stderr, "要删除指令的def是%s\n", inst->getDef()->toStr().c_str());
                fprintf(stderr, "删除指令%d\n", inst->getInstType());
            }
            inst = nextInst;
        }
    }

    // 处理不活跃的终结指令
    for (BasicBlock* bb : block_list) 
    {
        fprintf(stderr, "当前处理的块是！！！%d\n", bb->getNo());
        Instruction* termInst = bb->getTerminal();
        //fprintf(stderr, "当前处理终结指令所属块是！！！%d\n", termInst->getParent()->getNo());
        if (termInst && live.find(termInst) == live.end()) //如果一个块的终结指令被标记为不活跃
        {
            fprintf(stderr, "当前不活跃的终结指令所属块是%d\n", termInst->getParent()->getNo());
            // 查找第一个活跃的后继块
            BasicBlock* target = findFirstLiveSuccessor(bb, liveBlock);
            //fprintf(stderr, "当前不活跃的终结指令所属块的第一个活跃后继块是%d\n", target->getNo());
            if (target) 
            {
                // 替换不活跃的终结指令为跳转到第一个活跃块的指令   //替换指令不正确！！！！！
                UncondBrInstruction*  new_inst  = new UncondBrInstruction(target, bb);
                new_inst->save=true;   //！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

                //跳转指令变了，自然要更新前驱后继
                //bb->safeRemoveAllPred();
                bb->safeRemoveAllSucc();

                fprintf(stderr,"当前基本块的前驱和后继数目为: %d,%d\n", bb->getNumOfPred(),bb->getNumOfSucc());

                bb->remove(termInst);
                
                bb->addSucc(target);
                target->addPred(bb);

                fprintf(stderr,"当前块添加完前驱后继数量为%d,%d\n", bb->getNumOfPred(),bb->getNumOfSucc());
                fprintf(stderr, "替换不活跃的终结指令为跳转到块%d\n", target->getNo());
            }
            
        }
    }

    

    for(auto &bb:block_list)
    {
        bb->refresh();
    }

    
    removeUnreachableBlocks();
    


    fprintf(stderr, "函数%s激进死代码消除结束\n", sym_ptr->toStr().c_str());
}

// 查找第一个活跃的后继块
BasicBlock* Function::findFirstLiveSuccessor(BasicBlock* bb, const std::unordered_set<BasicBlock*>& liveBlock)
{
    std::unordered_set<BasicBlock*> visited;
    std::queue<BasicBlock*> queue;
    queue.push(bb);

    while (!queue.empty()) 
    {
        BasicBlock* current = queue.front();
        queue.pop();

        for (BasicBlock* succ : current->getSucc()) 
        {
            if (visited.find(succ) == visited.end()) 
            {
                visited.insert(succ);
                if (liveBlock.find(succ) != liveBlock.end()) 
                {
                    return succ;
                }
                queue.push(succ);
            }
        }
    }

    return nullptr;
}

// 创建反转后的控制流图
void Function::createReverseCFG() {
    // 反转前驱和后继关系
    for (auto &block : block_list) {
        block->getReversePred().clear();
        block->getReverseSucc().clear();

        for (auto &succ : block->getSucc()) {
            block->getReversePred().push_back(succ);
        }

        for (auto &pred : block->getPred()) {
            block->getReverseSucc().push_back(pred);
        }
    }

    // 设置反转后的入口基本块
    reverseEntry = exit;

    //输出一下反转后的CFG
    for (auto &block : block_list) {
        fprintf(stderr, "基本块%d的反转前驱有以下基本块\n", block->getNo());
        for (auto &pred : block->getReversePred()) {
            fprintf(stderr, "基本块%d\n", pred->getNo());
        }
        fprintf(stderr, "基本块%d的反转后继有以下基本块\n", block->getNo());
        for (auto &succ : block->getReverseSucc()) {
            fprintf(stderr, "基本块%d\n", succ->getNo());
        }
    }
}

void Function::sccp()
{
    fprintf(stderr, "开始执行函数%s常数传播\n", sym_ptr->toStr().c_str());


    bool needsccp=true;
    while(needsccp)
    {
        needsccp=false;
    

    std::set<BasicBlock *> v;   //用于记录已经访问过的基本块
    std::list<BasicBlock *> q;  //用于广度优先搜索
    q.push_back(entry);//将入口基本块加入队列
    v.insert(entry);//将入口基本块加入已访问集合
    while (!q.empty())//广度优先搜索
    {
        auto bb = q.front();//取出队列的第一个元素
        q.pop_front();//删除队列的第一个元素
       

        for (auto i = bb->getHead()->getNext(); i != bb->getHead(); i = i->getNext())
        {
            if(i->isBinary()&&dynamic_cast<BinaryInstruction*>(i)->canBeCalculated())
            {
                
                i->getDef()->replaceAllUsesWith(dynamic_cast<BinaryInstruction*>(i)->CalculatedResult());
                needsccp=true;
                i->save=false;
            }
            if(i->isCmp()&&dynamic_cast<CmpInstruction*>(i)->canBeCalculated())
            {
                needsccp=true;
                i->getDef()->replaceAllUsesWith(dynamic_cast<CmpInstruction*>(i)->CalculatedResult());
                i->save=false;
            }
            if(i->isZext()&&dynamic_cast<ZextInstruction*>(i)->canBeCalculated())
            {
                needsccp=true;
                i->getDef()->replaceAllUsesWith(dynamic_cast<ZextInstruction*>(i)->CalculatedResult());
                i->save=false;
            }

            if(i->isCond()&&dynamic_cast<CondBrInstruction*>(i)->getUse()[0]->getSymbolEntry()->isConstant())
            {
                needsccp=true;
                if(dynamic_cast<ConstantSymbolEntry*>(dynamic_cast<CondBrInstruction*>(i)->getUse()[0]->getSymbolEntry())->getValue())
                {
                    Instruction* uncond=new UncondBrInstruction(dynamic_cast<CondBrInstruction*>(i)->getTrueBranch(), bb);
                    uncond->save=true;
                    bb->removeSucc(dynamic_cast<CondBrInstruction*>(i)->getFalseBranch());
                    dynamic_cast<CondBrInstruction*>(i)->getFalseBranch()->removePred(bb);
                    i->save=false;
                }
                else
                {
                    Instruction* uncond=new UncondBrInstruction(dynamic_cast<CondBrInstruction*>(i)->getFalseBranch(), bb);
                    uncond->save=true;
                    bb->removeSucc(dynamic_cast<CondBrInstruction*>(i)->getTrueBranch());
                    dynamic_cast<CondBrInstruction*>(i)->getTrueBranch()->removePred(bb);
                    i->save=false;
                }
            }
        }

        bb->refresh();


       
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)//遍历基本块的后继
        {
            if (v.find(*succ) == v.end())//如果后继不在已访问集合中
            {
                v.insert(*succ);//将后继加入已访问集合
                q.push_back(*succ);//将后继加入队列
            }
        }
    }

    removeUnreachableBlocks();
    deleteSingalPHI();
    }
    
}

void Function::deleteSingalPHI()
{
    std::set<BasicBlock *> v;   //用于记录已经访问过的基本块
    std::list<BasicBlock *> q;  //用于广度优先搜索
    q.push_back(entry);//将入口基本块加入队列
    v.insert(entry);//将入口基本块加入已访问集合
    while (!q.empty())//广度优先搜索
    {
        auto bb = q.front();//取出队列的第一个元素
        q.pop_front();//删除队列的第一个元素
       

        for (auto i = bb->getHead()->getNext(); i != bb->getHead(); i = i->getNext())
        {
            if(i->isPhi())
            {
                PhiInstruction *PHIins=dynamic_cast<PhiInstruction*>(i);
                if(PHIins->incoming.size()==1)
                {
                    PHIins->getDef()->replaceAllUsesWith(PHIins->getUse()[0]);
                    PHIins->save=false;
                }

            }
        }
       bb->refresh();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)//遍历基本块的后继
        {
            if (v.find(*succ) == v.end())//如果后继不在已访问集合中
            {
                v.insert(*succ);//将后继加入已访问集合
                q.push_back(*succ);//将后继加入队列
            }
        }
    }

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


        // if(!(bb->rbegin()->isCond()||bb->rbegin()->isUncond()||bb==exit||bb->rbegin()->isRet()))
        // {
        //     if(bb->getNumOfPred()==0){}
        //     else{
        //         new UncondBrInstruction(exit, bb);//插入无条件跳转指令
        //         bb->addSucc(exit);//将出口基本块加入基本块的后继
        //         exit->addPred(bb);//将基本块加入出口基本块的前驱
        //     }
        // }
        // if(bb->empty()&&bb->getNumOfPred()!=0)//如果基本块为空
        // {
        //     new UncondBrInstruction(exit, bb);//插入无条件跳转指令
        //     bb->addSucc(exit);//将出口基本块加入基本块的后继
        //     exit->addPred(bb);//将基本块加入出口基本块的前驱
        // }
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
        fprintf(stderr,"开始输出支配树\n");
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
               
            if(i->getParent()->getNo()==0)
            {
                
                    continue;
            }
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
                fprintf(stderr,"基本块bb %d 的支配边界有 %d\n",bb->getNo(),df->getNo());
                if(phivisitedbb.find(df)==phivisitedbb.end())
                {
                    phivisitedbb.insert(df);
                    fprintf(stderr,"基本块bb %d 的支配边界有jin %d\n",bb->getNo(),df->getNo());
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

void Function::buildReverseDominanceTree(BasicBlock *ReverseDomTreeRoot)
{
        if (block_list.empty()) {
        return;
    }


    // 使用一个集合来计算每个基本块的支配集合
    for (auto& bb : block_list) {
        // 初始化每个基本块的支配前驱为空
        bb->setReverseDOMpred({});
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto& bb : block_list) {
            fprintf(stderr,"bb->getNo() = %d\n",bb->getNo());
            std::vector<BasicBlock*> newDOMpred;
            if (bb == ReverseDomTreeRoot) {
                // 入口基本块的支配前驱是它自己
                newDOMpred.push_back(bb);
            } else {
                // 对于每个基本块，计算其支配集合
                bool first = true;
                for (auto& predBB : bb->getReversePred()) {
                    if (first) {
                        for (auto& domPred : predBB->reverseDOMpred) {
                                newDOMpred.push_back(domPred);
                        }
                        first = false;
                    } else {
                        // 取前驱的交集
                        std::vector<BasicBlock*> intersection;
                        for (auto& domPred : predBB->reverseDOMpred) {
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
                if(std::find(newDOMpred.begin(), newDOMpred.end(), ReverseDomTreeRoot) == newDOMpred.end())
                    newDOMpred.push_back(ReverseDomTreeRoot);


            }

            // 检查是否需要更新支配前驱集合
            if (newDOMpred != bb->reverseDOMpred) {
                fprintf(stderr,"!!!bb->getNo() = %d\n",bb->getNo());
                bb->setReverseDOMpred(newDOMpred);
                changed = true;
            }
        }
    }
    
    fprintf(stderr,"支配前驱集合计算完成\n");
    
    //支配树的前驱排序
    for(auto &bb:block_list)
    {
        int len=int(bb->reverseDOMpred.size());
        for(int i=0;i<len;i++)
        {
            for(int j=i+1;j<len;j++)
            {
                if(std::find(bb->reverseDOMpred[i]->reverseDOMpred.begin(),bb->reverseDOMpred[i]->reverseDOMpred.end(),bb->reverseDOMpred[j])==bb->reverseDOMpred[i]->reverseDOMpred.end())
                {
                    BasicBlock *temp=bb->reverseDOMpred[i];
                    bb->reverseDOMpred[i]=bb->reverseDOMpred[j];
                    bb->reverseDOMpred[j]=temp;
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
        if(bb->reverseDOMpred.size()<=1)
        {
            parent = nullptr;
        }
        else
        {
            parent = bb->reverseDOMpred[1];
        }
        if (parent != nullptr) {
        fprintf(stderr,"1\n");
        fprintf(stderr,"parent->no = %d\n",parent->getNo());
        fprintf(stderr,"reverseDOMsucc.size() = %ld\n",parent->reverseDOMsucc.size());
        parent->reverseDOMsucc.push_back(bb);
        fprintf(stderr,"%d的支配后继有%d\n",parent->getNo(),bb->getNo());

        
        fprintf(stderr,"1=2\n");

        }

    }
    

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
                        for(auto &j:node->getReverseSucc())
                        {
                            if(j==bb||std::find(j->reverseDOMpred.begin(),j->reverseDOMpred.end(),bb)==j->reverseDOMpred.end())
                            {
                                bb->reverseDomFrontier.push_back(j);
                            }
                        }
                      
                        // 将当前节点的所有子节点加入队列
                        for (auto& child : node->reverseDOMsucc) {
                            q.push(child);
                        }
                    }

            }

    }

}


// 输出支配树
void Function::printReverseDominanceTree(FILE* out) {
    fprintf(stderr,"反支配树输出\n");
        for (auto& bb : block_list) {
        fprintf(stderr,"bb->getNo()是 %d，他的支配前驱有 ",bb->getNo());
        for (auto& predBB : bb->reverseDOMpred) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");
    }

        // 计算支配树的后继节点
    for (auto& bb : block_list) {
        fprintf(stderr,"bb->getNo()是 %d，他的支配后继有 ",bb->getNo());
        for (auto& BB : bb->reverseDOMsucc) {
            fprintf(stderr,"%d ",BB->getNo());
            
        }
        fprintf(stderr,"\n");
        fprintf(stderr,"bb->getNo()是 %d，他的支配边界有 ",bb->getNo());
        for (auto& predBB : bb->reverseDomFrontier) {
            fprintf(stderr,"%d ",predBB->getNo());
            
        }
        fprintf(stderr,"\n");

    }

}




void Function::genMachineCode(AsmBuilder* builder) 
{
    auto cur_unit = builder->getUnit();
    auto cur_func = new MachineFunction(cur_unit, this->sym_ptr);
    builder->setFunction(cur_func);

    // 处理函数参数
    for (long unsigned int param_index = 0; param_index < this->params.size(); param_index++) {
        MachineOperand* machine_param;
        if (param_index < 4) {
            // 前四个参数依次放入 r0, r1, r2, r3
            machine_param = new MachineOperand(MachineOperand::REG, param_index);
        } else {
            // 超过四个参数的部分可以根据需要处理
            // 这里假设超过四个参数的部分不需要处理
            
        }
        cur_func->addParam(machine_param);
    }


    
    // 遍历所有基本块，生成机器码
    std::map<BasicBlock*, MachineBlock*> map;
    for(auto block : block_list)
    {
        block->genMachineCode(builder);
        if(this->return_val==nullptr)
        {
            auto bx_inst = new BranchMInstruction(builder->getBlock(), BranchMInstruction::BX, new MachineOperand(MachineOperand::REG, 14));
            builder->getBlock()->InsertInst(bx_inst);
        }

        map[block] = builder->getBlock();

    }


    // Add pred and succ for every block
    for(auto block : block_list)
    {
        auto mblock = map[block];
        for (auto pred = block->pred_begin(); pred != block->pred_end(); pred++)
            mblock->addPred(map[*pred]);
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
            mblock->addSucc(map[*succ]);
    }
    cur_unit->InsertFunc(cur_func);

    


    // // 创建第一个基本块
    // //auto entry_block = new MachineBlock(cur_func, entry->getNo()); // 假设基本块编号为 0
    // MachineBlock* entry_block = cur_func->getBlockByNo(entry->getNo());  //拿出entry机器块
    // fprintf(stderr,"entry_block的编号是%d\n",entry_block->getNo());
    // builder->setBlock(entry_block); // 将 entry_block 设置为当前基本块
    // cur_func->InsertBlock(entry_block); // 插入到当前函数的基本块列表中
    
    // int vreg_counter = 0;  // 用于直接分配虚拟寄存器编号

    // // 处理函数参数
    // for (long unsigned int param_index = 0; param_index < this->params.size(); param_index++) // 基于索引的循环
    // {
    //     // 如果参数在 r0-r3 中，用寄存器直接传递
    //     MachineOperand* param_operand;
    //     if (param_index < 4) {
    //         param_operand = new MachineOperand(MachineOperand::REG, param_index); // r0, r1, r2, r3
    //     } else {
    //         // 超过 r3 的参数从栈中加载
    //         int stack_offset = (param_index - 4) * 4;
    //         param_operand = new MachineOperand(MachineOperand::IMM, stack_offset);
    //     }

    //     // 分配虚拟寄存器
    //     auto param_vreg = new MachineOperand(MachineOperand::VREG, vreg_counter++); // 可以直接++，C++支持后置++
    //     auto cur_block = builder->getBlock();
    //     fprintf(stderr,"当前cur_block是%s\n",cur_block->getParent()->getSymbolEntry()->toStr().c_str());

    //     if (param_index < 4) {
    //         // 参数在寄存器中，直接生成 MOV 指令
    //         fprintf(stderr,"参数在寄存器中\n");
    //         auto mov_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, param_vreg, param_operand);
    //         fprintf(stderr,"是在这里吗\n");
    //         fprintf(stderr,"当前cur_block是%s\n",cur_block->getParent()->getSymbolEntry()->toStr().c_str());
    //         cur_block->InsertFront(mov_inst);
    //         fprintf(stderr,"在寄存器中参数已放入\n");
    //     } else {
    //         // 参数在栈中，生成 LOAD 指令
    //         fprintf(stderr,"参数已超过3个，剩下的参数在栈中\n");
    //         auto load_inst = new LoadMInstruction(cur_block, param_vreg, new MachineOperand(MachineOperand::REG, 13), param_operand); //  sp 在 r13
    //         cur_block->InsertFront(load_inst);
    //     }

    //     // 将虚拟寄存器存储到函数参数映射
    //     cur_func->addParam(param_vreg);
    // }



}
