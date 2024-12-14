#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include "BasicBlock.h"
#include "SymbolTable.h"
#include <unordered_map>
class Unit;

class Function
{
    typedef std::vector<BasicBlock *>::iterator iterator;
    typedef std::vector<BasicBlock *>::reverse_iterator reverse_iterator;

private:
    std::vector<BasicBlock *> block_list;//基本块列表
    SymbolEntry *sym_ptr;//符号表项
    Operand *return_val;//返回值操作数
    BasicBlock *entry;//入口基本块
    BasicBlock *exit;//出口基本块
    BasicBlock *DomTreeRoot;//支配树根节点  //支配树根节点是支配树的根节点，它是支配树中的唯一一个没有前驱的节点
    Unit *parent;//Unit *parent 成员变量用于指向包含该函数的 Unit 对象，即表示该函数所属的编译单元。
    //通过 parent 指针，Function 对象可以访问其所属的 Unit 对象。这有助于组织和管理编译单元中的所有函数和全局变量
    //parent 指针提供了函数所属的编译单元的上下文信息，使得函数可以访问和操作编译单元中的其他信息，例如全局变量列表、其他函数等。
    std::vector<Operand *> params; // 参数列表

public:
    bool isallocaOperand(Operand *op);
    void renameBlocks(BasicBlock* bb);
    void removeUnreachableBlocks();
    std::vector<Operand *> allocaOperands;
    BasicBlock *getExit() { return exit; };
    Operand *getRetValue() { return return_val; };
    std::vector<BasicBlock *> while_cond;//在这个函数中while循环条件栈
    std::vector<BasicBlock *> while_end;//在这个函数中while循环结束栈
    Function(Unit *, SymbolEntry *);
    ~Function();
    void insertBlock(BasicBlock *bb);
    //获取入口基本块
    BasicBlock *getEntry() { return entry; };
    void remove(BasicBlock *bb);
    void output() const;
    void optimize();
    std::vector<BasicBlock *> &getBlockList(){return block_list;};
    iterator begin() { return block_list.begin(); };
    iterator end() { return block_list.end(); };
    reverse_iterator rbegin() { return block_list.rbegin(); };
    reverse_iterator rend() { return block_list.rend(); };
    SymbolEntry *getSymPtr() { return sym_ptr; };
    void addParam(Operand *param) { params.push_back(param); };
    std::vector<Operand *> &getParams() { return params; };
    void buildDominanceTree();
    void printDominanceTree(FILE* out = stderr); // 输出支配树
    void PHIoptimize();
    void deadCodeElimination(); //死代码消除优化
    void aggressiveDeadCodeElimination();
};

#endif
