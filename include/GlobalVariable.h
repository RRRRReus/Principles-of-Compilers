#ifndef __GLOBALVARIABLE_H__
#define __GLOBALVARIABLE_H__

#include "Type.h"
#include "SymbolTable.h"
#include "AsmBuilder.h"

class GlobalVariable
{
private:
    SymbolEntry *se;
    

public:
    GlobalVariable(SymbolEntry *se) : se(se) {}
    void output() const;
    void optimize() const;
    void genMachineCode(AsmBuilder*);
};

#endif