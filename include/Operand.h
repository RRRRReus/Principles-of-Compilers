#ifndef __OPERAND_H__
#define __OPERAND_H__

#include "SymbolTable.h"
#include <vector>

class Instruction;
class Function;


// class Operand - The operand of an instruction.   //指令的操作数。
class Operand
{
typedef std::vector<Instruction *>::iterator use_iterator;

private:
    Instruction *def;                // The instruction where this operand is defined. 定义此操作数的指令。
    std::vector<Instruction *> uses; // Intructions that use this operand. 使用此操作数的指令。
    SymbolEntry *se;                 // The symbol entry of this operand. 此操作数的符号表项。
    bool isUndef=false;//是否未定义
public:
    Operand(SymbolEntry*se) :se(se), isUndef(false) {def = nullptr;};
    void setDef(Instruction *inst) {def = inst;};//设置定义操作数
    void addUse(Instruction *inst) { uses.push_back(inst);};
    void removeUse(Instruction *inst);
    void cleanUse() {uses.clear();};
    int usersNum() const {return uses.size();};//返回使用此操作数的指令数量
    Instruction *getDef() { return def; };//获取定义操作数
    std::vector<Instruction *> &getUse() { return uses; };
    SymbolEntry *getSymbolEntry() { return se; };

    use_iterator use_begin() {return uses.begin();};
    use_iterator use_end() {return uses.end();};
    Type* getType() {return se->getType();};
    std::string toStr() const;//返回字符串

    void setUndef() {isUndef=true;};//设置为未定义
    bool isUndefOperand() {return isUndef;};//是否为未定义操作数

    void replaceAllUsesWith(Operand *newOperand);//替换所有使用
};

#endif