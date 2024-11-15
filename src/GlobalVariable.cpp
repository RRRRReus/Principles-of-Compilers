
#include "GlobalVariable.h"
#include <cstdio>

extern FILE *yyout;

void GlobalVariable::output() const
{
    // @变量名 = global i32 0, align 4
    // 获取初始值
    fprintf(stderr, "进入GlobalVariable::output()\n");
    std::string initialValue = static_cast<IdentifierSymbolEntry*>(se)->getInitialValue();
    fprintf(stderr, "已获取全局变量初始值%s，\n", initialValue.c_str());
    if (initialValue.empty())
    {
        initialValue = "0"; // 默认初始值
    }
    fprintf(yyout, "%s = global %s %s, align 4\n", se->toStr().c_str(), se->getType()->toStr().c_str(), initialValue.c_str());
}