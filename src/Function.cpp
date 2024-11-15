#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);//插入unit的函数列表
    entry = new BasicBlock(this);//创建一个新的基本块
    sym_ptr = s;//符号表项
    parent = u;
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
    printf("已输出define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    std::set<BasicBlock *> v;   //用于记录已经访问过的基本块
    std::list<BasicBlock *> q;  //用于广度优先搜索
    q.push_back(entry);//将入口基本块加入队列
    v.insert(entry);//将入口基本块加入已访问集合
    while (!q.empty())//广度优先搜索
    {
        auto bb = q.front();//取出队列的第一个元素
        q.pop_front();//删除队列的第一个元素
        if(!bb->empty())
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
