#include "Unit.h"
#include <iostream>
extern FILE* yyout;

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);//将函数f加入函数列表
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    //std::string target= "target triple = \"x86_64-pc-linux-gnu\"\n";
    fprintf(yyout, "target triple = \"armv7-unknown-linux-gnueabihf\"\n");

    for (auto global : global_list)
        global->output();

    for (auto &func : func_list)
        func->output();

    fprintf(yyout, "declare i32 @getint()\n");
    fprintf(yyout, "declare void @putint(i32)\n");
    fprintf(yyout, "declare void @putch(i32)\n");
    fprintf(yyout, "declare void @putarray(i32, i32*)\n");
    fprintf(yyout, "declare i32 @getfarray(float*)\n");
    fprintf(yyout, "declare void @putfarray(i32, float*)\n");

    


    
    //fprintf(yyout, "declare void @putint(i32) \n");

    fprintf(yyout, "declare i32 @getch()\n");
    fprintf(yyout, "declare void @putfloat(float)\n");
    fprintf(yyout, "declare float @getfloat()\n");

}
void Unit::optimize()
{
    fprintf(stderr, "编译单元优化\n");
    for (auto global : global_list)
        global->optimize();

    for (auto &func : func_list)
        func->optimize();
}

Unit::~Unit()
{
    auto delete_list_func = func_list;
    for(auto &func:delete_list_func)
        delete func;
    auto delete_list_global = global_list;
    for (auto global : delete_list_global)
        delete global;
}

//新增全局变量内容
void Unit::insertGlobal(GlobalVariable *global)
{
    global_list.push_back(global);
}

void Unit::removeGlobal(GlobalVariable *global)
{
    global_list.erase(std::remove(global_list.begin(), global_list.end(), global), global_list.end());
}
