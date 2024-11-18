#include "BasicBlock.h"
#include "Function.h"
#include <algorithm>

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
        fprintf(stderr,"pred不为空\n");
        fprintf(yyout, "%*c; preds = %%B%d", 32, '\t', pred[0]->getNo());
        for (auto i = pred.begin() + 1; i != pred.end(); i++)
            fprintf(yyout, ", %%B%d", (*i)->getNo());
    }
    fprintf(yyout, "\n");
    fprintf(stderr,"已输出B%d,开始遍历指令链表:\n", no);
    fprintf(stderr,"head->getNext()是否等于head:%d\n",head->getNext()==head);
    for (auto i = head->getNext(); i != head; i = i->getNext()){
        //fprintf(stderr,"进来了吗\n");
        //fprintf(stderr,"i的指令类型是%s\n",i->getDef()->toStr().c_str());
        //注意注意！！！getDef()是获取操作数不是类型！！操作数可能为空！！类型是instype
        i->output();
    }
        
}
//添加后继
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
}

BasicBlock::BasicBlock(Function *f)
{
    this->no = SymbolTable::getLabel();
    f->insertBlock(this);
    parent = f;
    head = new DummyInstruction();
    head->setParent(this);
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
