
#include "GlobalVariable.h"
#include <cstdio>
#include <cstring>
#include <inttypes.h> // 添加这个头文件以使用 PRIx64

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

    // bool isInteger = true;
    // for (char c : initialValue)
    // {
    //     if (!isdigit(c) && c != '-')
    //     {
    //         isInteger = false;
    //         break;
    //     }
    // }
    // if(isInteger){
    //     fprintf(yyout, "%s = global %s %s, align 4\n", se->toStr().c_str(), se->getType()->toStr().c_str(), initialValue.c_str());
    // }
    // else{//除了float还有其他情况吗？
    // double doubleValue = std::stod(initialValue);
    // uint64_t ieee754Value;
    // memcpy(&ieee754Value, &doubleValue, sizeof(doubleValue));
    // fprintf(yyout, "%s = global %s 0x%016" PRIx64 ", align 8\n", se->toStr().c_str(), se->getType()->toStr().c_str(), ieee754Value);
    // }

    //是否常量
    std::string type = se->getType()->toStr();
    if(se->getType()->isPtr())
    {
        type=dynamic_cast<PointerType*>(se->getType())->getValueType()->toStr();
    }

    
    if(se->getType()->getConst()){
        fprintf(yyout, "%s = constant %s %s, align 4\n", se->toStr().c_str(), type.c_str(), initialValue.c_str());
    }
    else{
        fprintf(yyout, "%s = global %s %s, align 4\n", se->toStr().c_str(), type.c_str(), initialValue.c_str());
    }

    
}