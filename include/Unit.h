#ifndef __UNIT_H__
#define __UNIT_H__

#include <vector>
#include "Function.h"
#include "GlobalVariable.h"

class Unit
{
    typedef std::vector<Function *>::iterator iterator;
    typedef std::vector<Function *>::reverse_iterator reverse_iterator;
    typedef std::vector<GlobalVariable *>::iterator global_iterator;
    typedef std::vector<GlobalVariable *>::reverse_iterator global_reverse_iterator;

private:
    std::vector<Function *> func_list;  //函数列表
    std::vector<GlobalVariable *> global_list; // 全局变量列表  //新增全局变量内容
public:
    Unit() = default;
    ~Unit() ;
    void insertFunc(Function *);//插入函数
    void removeFunc(Function *);
    void insertGlobal(GlobalVariable *);//新增全局变量内容，插入全局变量
    void removeGlobal(GlobalVariable *);
    void output() const;//输出函数，输出到.ll文件中
    void optimize();//优化函数
    iterator begin() { return func_list.begin(); };
    iterator end() { return func_list.end(); };
    reverse_iterator rbegin() { return func_list.rbegin(); };
    reverse_iterator rend() { return func_list.rend(); };

    global_iterator global_begin() { return global_list.begin(); }
    global_iterator global_end() { return global_list.end(); }
    global_reverse_iterator global_rbegin() { return global_list.rbegin(); }
    global_reverse_iterator global_rend() { return global_list.rend(); }
};

#endif