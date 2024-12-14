#include "Operand.h"
#include <sstream>
#include <algorithm>
#include <string.h>
#include "BasicBlock.h"
std::string Operand::toStr() const
{
    return se->toStr();
}

void Operand::reName(Operand *newName)
{
    se = newName->getSymbolEntry();
}

void Operand::reNameInBB(Operand *newName, BasicBlock *bb)
{
    for(Instruction* i:uses)
    {
        if(i->getParent() == bb)
        {
            
        }
    }
}

SymbolEntry* Operand::newName()
{
    counter++;
    SymbolEntry *newname=new TemporarySymbolEntry(this->se->getType(),SymbolTable::getLabel());
    NameStack.push_back(newname);
    return newname;

}

void Operand::refreshName()
{
    se=NameStack.back();
}

void Operand::removeUse(Instruction *inst)
{
    auto i = std::find(uses.begin(), uses.end(), inst);
    if(i != uses.end())
        uses.erase(i);
}

