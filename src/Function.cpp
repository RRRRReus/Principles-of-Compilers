#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

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

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    fprintf(stderr,"已输出define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    std::set<BasicBlock *> v;   //用于记录已经访问过的基本块
    std::list<BasicBlock *> q;  //用于广度优先搜索
    q.push_back(entry);//将入口基本块加入队列
    v.insert(entry);//将入口基本块加入已访问集合
    while (!q.empty())//广度优先搜索
    {
        auto bb = q.front();//取出队列的第一个元素
        q.pop_front();//删除队列的第一个元素
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
