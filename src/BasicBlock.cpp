#include "BasicBlock.h"
#include "Function.h"
#include <algorithm>
#include <unordered_map>
#include <stack>
#include "Type.h"

extern FILE* yyout;

// insert the instruction to the front of the basicblock.
void BasicBlock::insertFront(Instruction *inst)
{
    insertBefore(inst, head->getNext());
}

// insert the instruction to the back of the basicblock.
void BasicBlock::insertBack(Instruction *inst) 
{
    insertBefore(inst, head);
}

// insert the instruction dst before src.   //在src之前插入指令dst
void BasicBlock::insertBefore(Instruction *dst, Instruction *src)
{
    // Todo
    dst->setPrev(src->getPrev());// 更新 dst 的前驱和后继指针
    dst->setNext(src);

    // 更新 src 的前驱指针
    src->setPrev(dst);

    // 更新 src 前驱的后继指针
    if (dst->getPrev() != nullptr) {
        dst->getPrev()->setNext(dst);
    }

    // 设置 dst 的父基本块
    dst->setParent(this);
}

// remove the instruction from intruction list.
void BasicBlock::remove(Instruction *inst)
{
    inst->getPrev()->setNext(inst->getNext());
    inst->getNext()->setPrev(inst->getPrev());
}

void BasicBlock::output() const
{
    fprintf(yyout, "B%d:", no);//输出基本块的编号

    if (!pred.empty())//如果前驱不为空，则输出前驱
    {
        fprintf(yyout, "%*c; preds = %%B%d", 32, '\t', pred[0]->getNo()); //块前驱提示内容
        for (auto i = pred.begin() + 1; i != pred.end(); i++)
            fprintf(yyout, ", %%B%d", (*i)->getNo());
    }
    fprintf(yyout, "\n");
    for (auto i = head->getNext(); i != head; i = i->getNext()){
        //fprintf(stderr,"进来了吗\n");
        //fprintf(stderr,"i的指令类型是%d\n",i->getInstType());
        //注意注意！！！getDef()是获取操作数不是类型！！操作数可能为空！！类型是instype
        i->output();
    }
        
}
void BasicBlock::optimize()
{
    fprintf(stderr, "基本块%d优化\n", no);

    std::unordered_map<Operand*, Operand*> valMap;//用于samebb中，若alloca指令的use和def都在同一个基本块中
    Operand *allocaDef=nullptr;//用于alloca指令的优化

    for (auto i = head->getNext(); i != head; i = i->getNext())//遍历基本块中的指令
    {
        fprintf(stderr,"进入循环\n");
        i->optimize();
        i->save=true;


        if(i->isAlloca())
        {
            fprintf(stderr,"遇到了alloca指令\n");
            AllocaInstruction *allocaInst = dynamic_cast<AllocaInstruction*>(i);
            fprintf(stderr,"alloca指令是不是int数组%d\n",allocaInst->getSymbolEntry()->getType()->isIntArray());
            fprintf(stderr,"alloca指令是不是指针类型%d\n",allocaInst->getDef()->getType()->isPtr());
            if(allocaInst->getDef()->usersNum()==0)//如果alloca的def的用户数为0，则将save置为false
            {
                i->save=false;
            }
            else if(!allocaInst->getSymbolEntry()->getType()->isIntArray() && !allocaInst->getSymbolEntry()->getType()->isFloatArray())//如果是数组,则不进行优化
            {
                fprintf(stderr,"当前指令的def的用户数不为0，且存在同块操作数\n");
                // 检查 alloca 的 use 和 def 是否都在同一基本块内
                bool allUsesInSameBB = true;//假设use和def都在同一个基本块中
                fprintf(stderr,"当前alloca指令的作用对象是%s\n",dynamic_cast<AllocaInstruction*>(i)->getDef()->toStr().c_str());
                Operand *allocaDef_current = dynamic_cast<AllocaInstruction*>(i)->getDef();//获取alloca的def

                for(auto use : allocaDef_current->getUse())//遍历所有用到def的指令
                {
                    //比较两个基本块的编号是否相等，这么比较应该正确吧
                    if(use->getParent()->getNo()!=this->getNo())//如果use的父基本块不是当前基本块
                    {
                        allUsesInSameBB=false;
                        break;
                    }
                }

                if(allUsesInSameBB)//如果use和def都在同一个基本块中
                {
                    i->save=false;
                    // 初始化 val 为 undef
                    allocaDef = allocaDef_current;//确定这个指针就是我们要找的同块操作数
                    fprintf(stderr,"找到了在同一个基本块的操作数%s\n",allocaDef->toStr().c_str());
                    Operand *val = new Operand(new TemporarySymbolEntry(allocaDef->getType(), SymbolTable::getLabel()));
                    fprintf(stderr,"val是%s\n",val->toStr().c_str());
                    val->setUndef();//设置为未定义操作数 
                    valMap[allocaDef]=val;//将val和allocaDef放入map中
           
                }

            }
        }

        if(i->isStore())
        {
            // fprintf(stderr,"基本块%d中的指令是alloca\n",no);
            // fprintf(stderr,"基本块%d中的指令的def的用户数是%d\n",no,dynamic_cast<AllocaInstruction*>(i)->getDef()->usersNum());
            fprintf(stderr,"遇到了store指令\n");
            if(dynamic_cast<StoreInstruction*>(i)->getDef()->usersNum()==0)
            {
                i->save=false;
            }
            else if(valMap.find(dynamic_cast<StoreInstruction*>(i)->getDef())!=valMap.end())//store指令要给同块操作数赋值
            {
                fprintf(stderr,"store指令要给同块操作数赋值\n");
                //若是store指令在给同块操作数赋值，则将要赋的值直接给val,并将save置为false
                // 将 store 指令要写入的值 设为 val，并删除 store 指令
                Operand *src=dynamic_cast<StoreInstruction*>(i)->getUse()[1];//获取store指令的src
                //dynamic_cast<StoreInstruction*>(i)->setOperand(src,val);
                valMap[dynamic_cast<StoreInstruction*>(i)->getDef()] = src;//直接这么赋值对吗？

                i->save=false;
            }
            else if(valMap.find(dynamic_cast<StoreInstruction*>(i)->getUse()[1])!=valMap.end())//store指令要用同块操作数给其他操作数赋值
            {
                fprintf(stderr,"store指令要用同块操作数给其他操作数赋值\n");
                //若是store指令在给同块操作数赋值，则将要赋的值直接给val,并将save置为false
                fprintf(stderr,"找到了store指令取出没用操作数%s\n",dynamic_cast<StoreInstruction*>(i)->getDef()->toStr().c_str());
                fprintf(stderr,"当前store指令的def是%s\n",dynamic_cast<StoreInstruction*>(i)->getDef()->toStr().c_str());
                fprintf(stderr,"当前store指令的src是%s\n",dynamic_cast<StoreInstruction*>(i)->getUse()[1]->toStr().c_str());
                // store 指令要将 val 写入它要赋值的其他操作数，并删除 store 指令
                Operand *val = valMap[dynamic_cast<StoreInstruction*>(i)->getUse()[1]];//获取val
                dynamic_cast<StoreInstruction*>(i)->setDef(val);//获取store指令的def

                i->save=false;
            }
        }

        if(i->isLoad())
        {
            //若遇见load指令，发现load的operand[1]指向的内存地址是一个同块操作数，则需要将load之后存放的值改为val，即将operand[0]改为val
            fprintf(stderr,"遇见了load指令\n");
            Operand *loadSrc=dynamic_cast<LoadInstruction*>(i)->getUse()[0];//获取load指令的def

            //fprintf(stderr,"load指令getDef之后的结果是%s\n",dynamic_cast<LoadInstruction*>(i)->getDef()->toStr().c_str());
            if(valMap.find(loadSrc) != valMap.end())//如果val不为空且load的def等于alloca的def
            {
                //如果该load指令是要去取之前找到的val，那么将load的def替换为val
                Operand *val = valMap[loadSrc];
                Operand *loadDef = dynamic_cast<LoadInstruction*>(i)->getDef();
                loadDef->replaceAllUsesWith(val);//将所有用到此load的def的操作数替换为val
                fprintf(stderr,"load指令的def被替换为val:%s\n",dynamic_cast<LoadInstruction*>(i)->getDef()->toStr().c_str());
                i->save=false;
            }
        }

        if(i->isCond())
        {
            fprintf(stderr,"遇到了条件跳转指令\n");
            BasicBlock* true_bb=dynamic_cast<CondBrInstruction*>(i)->getTrueBB();
            BasicBlock* false_bb=dynamic_cast<CondBrInstruction*>(i)->getFlaseBB();
            
            this->addSucc(true_bb);
            true_bb->addPred(this);

            this->addSucc(false_bb);
            false_bb->addPred(this);
            
            break;
        }

        if(i->isUncond())
        {
            fprintf(stderr,"遇到了无条件跳转指令\n");
            BasicBlock* uncond_bb=dynamic_cast<UncondBrInstruction*>(i)->getBranchBB();
            this->addSucc(uncond_bb);
            uncond_bb->addPred(this);
            break;
        }

        //head=optimizeHead;
    }

    fprintf(stderr,"基本块%d指令循环遍历结束\n",no);
    
    Instruction *next;
    for (auto i = head->getNext(); i != head; i = next)
    {
        next=i->getNext();

        if(i->save)//如果save为true，则将指令插入到优化后的链表中
            insertBefore(i,optimizeHead);
    }
    head=optimizeHead;
    //refresh();

}
// 添加后继
void BasicBlock::addSucc(BasicBlock *bb)
{
    succ.push_back(bb);
}

// remove the successor basicclock bb.
void BasicBlock::removeSucc(BasicBlock *bb)
{
    succ.erase(std::find(succ.begin(), succ.end(), bb));
}
//添加前驱
void BasicBlock::addPred(BasicBlock *bb)
{
    pred.push_back(bb);
}

// remove the predecessor basicblock bb.
void BasicBlock::removePred(BasicBlock *bb)
{
    pred.erase(std::find(pred.begin(), pred.end(), bb));
    fprintf(stderr,"开始处理phi前驱清理\n");
    Instruction *next;
  //fprintf(stderr,"head是%d\n",head->getInstType());
  for (auto i = head->getNext(); i != head; i = next)
  {
    if(i->isPhi())
    {
        fprintf(stderr,"遇到了phi指令\n");
        PhiInstruction *phiInst = dynamic_cast<PhiInstruction*>(i);
        fprintf(stderr,"删之前phi指令的incoming数目是%ld\n",phiInst->incoming.size());

        phiInst->removeIncoming(bb);
        fprintf(stderr,"删之后phi指令的incoming数目是%ld\n",phiInst->incoming.size());

    }
    next=i->getNext();

  }
}

void BasicBlock::safeRemoveAllSucc()
{
    for(auto &bb:succ)
        bb->removePred(this);
    succ.clear();
}

void BasicBlock::safeRemoveAllPred()
{
    for(auto &bb:pred)
        bb->removeSucc(this);
    pred.clear();
}

Instruction *BasicBlock::getTerminal()//获取基本块的终结指令
{
    if(empty()){
        return nullptr;
    }
    Instruction* lastInst = rbegin();
    if(lastInst->isUncond() || lastInst->isCond() || lastInst->isRet())
    {
        return lastInst;
    }
    return nullptr;
}


void BasicBlock::refresh()
{
    optimizeHead=new DummyInstruction();
  //int k=0;
  fprintf(stderr,"基本块%d的刷新开始\n",no);
  Instruction *next;
  //fprintf(stderr,"head是%d\n",head->getInstType());
  for (auto i = head->getNext(); i != head; i = next)
  {

    //fprintf(stderr,"基本块%d的指令类型是%d\n",no,i->getInstType());
    //std::cin>>k;
    next=i->getNext();

    if(i->save)
    {
      insertBefore(i,optimizeHead);
      //fprintf(stderr,"指令是保存的\n");
    }
    else
    {
      //fprintf(stderr,"指令是不保存的\n");
    }
    // if(i->isUncond()||i->isCond()||i->isRet())
    //   {
    //     fprintf(stderr,"基本块%d的指令是无条件分支\n",no);
    //     fprintf(stderr,"这条指令的next是%d\n",i->getNext()->getInstType());
    //     fprintf(stderr,"next是%d\n",next->getInstType());
    //     break;
    //   }

  }
  fprintf(stderr,"基本块%d的刷新结束\n",no);
  head=optimizeHead;
    
}

void BasicBlock::genMachineCode(AsmBuilder* builder) 
{
    auto cur_func = builder->getFunction();
    builder->setFunction(cur_func);
    auto cur_block = new MachineBlock(cur_func, no);
    builder->setBlock(cur_block);
    for (auto i = head->getNext(); i != head; i = i->getNext())
    {
        i->genMachineCode(builder);
    }
    cur_func->InsertBlock(cur_block);
}

BasicBlock::BasicBlock(Function *f)
{
    this->no = SymbolTable::getLabel();
    f->insertBlock(this);
    parent = f;
    head = new DummyInstruction();
    head->setParent(this);
    optimizeHead =new DummyInstruction();
    optimizeHead->setParent(this);

}

BasicBlock::~BasicBlock()
{
    Instruction *inst;
    inst = head->getNext();
    while (inst != head)
    {
        Instruction *t;
        t = inst;
        inst = inst->getNext();
        delete t;
    }
    for(auto &bb:pred)
        bb->removeSucc(this);
    for(auto &bb:succ)
        bb->removePred(this);
    parent->remove(this);
}
