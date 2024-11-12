#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include "BasicBlock.h"
#include "SymbolTable.h"

class Unit;

class Function
{
    typedef std::vector<BasicBlock *>::iterator iterator;
    typedef std::vector<BasicBlock *>::reverse_iterator reverse_iterator;

private:
    std::vector<BasicBlock *> block_list;//基本块列表
    SymbolEntry *sym_ptr;//符号表项
    BasicBlock *entry;//入口基本块
    Unit *parent;//父类

public:
    std::vector<BasicBlock *> while_cond;//在这个函数中while循环条件栈
    std::vector<BasicBlock *> while_end;//在这个函数中while循环结束栈
    Function(Unit *, SymbolEntry *);
    ~Function();
    void insertBlock(BasicBlock *bb) { block_list.push_back(bb); };
    BasicBlock *getEntry() { return entry; };//获取入口基本块
    void remove(BasicBlock *bb);
    void output() const;
    std::vector<BasicBlock *> &getBlockList(){return block_list;};
    iterator begin() { return block_list.begin(); };
    iterator end() { return block_list.end(); };
    reverse_iterator rbegin() { return block_list.rbegin(); };
    reverse_iterator rend() { return block_list.rend(); };
    SymbolEntry *getSymPtr() { return sym_ptr; };
};

#endif
