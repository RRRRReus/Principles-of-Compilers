
#include "GlobalVariable.h"
#include <cstdio>

extern FILE *yyout;

void GlobalVariable::output() const
{
    // @变量名 = global i32 0, align 4
    // 获取初始值
    std::string initialValue = static_cast<IdentifierSymbolEntry*>(se)->getInitialValue();
    if (initialValue.empty())
    {
        initialValue = "0"; // 默认初始值
    }
    fprintf(yyout, "%s = global %s, align 4\n", se->toStr().c_str(), se->getType()->toStr().c_str());
    //fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}