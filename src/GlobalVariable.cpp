
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

void GlobalVariable::optimize() const
{
    fprintf(stderr, "全局变量%s优化\n", se->toStr().c_str());
}

void GlobalVariable::genMachineCode(AsmBuilder* builder)
{
    fprintf(stderr,"进入GlobalVariable::genMachineCode()\n");
    // 获取当前的 MachineUnit
    auto cur_unit = builder->getUnit();

    // 获取全局变量的名称，并去掉第一个字符 '@'
    std::string varName = se->toStr();
    if (!varName.empty() && varName[0] == '@') {
        varName = varName.substr(1);
    }

    // 创建一个 LABEL 类型的 MachineOperand，用于表示全局变量
    MachineOperand* globalVarOperand = new MachineOperand(varName);

   if(se->getType()->getConst())
   {
         globalVarOperand->setGlobalConst(true);
         fprintf(stderr,"我们已将全局常量bool值改变为 %d\n",globalVarOperand->isGlobalConst());
         fprintf(stderr, "全局变量%s是常量!!!!!!!!!!!!!!!!!!!!!!\n", varName.c_str());
   }

    // 获取全局变量的初始值
    std::string initialValue = static_cast<IdentifierSymbolEntry*>(se)->getInitialValue();
    if (initialValue.empty())
    {
        initialValue = "0"; // 默认初始值
    }

    globalVarOperand->setInitialValue(initialValue);
    fprintf(stderr, "全局变量%s的初始值为%s\n", varName.c_str(), initialValue.c_str());
    cur_unit->InsertGlobalVar(globalVarOperand);   // 将全局变量插入到 MachineUnit 中


    fprintf(stderr,"退出GlobalVariable::genMachineCode()\n");
}