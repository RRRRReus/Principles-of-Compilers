#include "SymbolTable.h"
#include "Type.h"
#include <iostream>
#include <sstream>

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, float fvalue): SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->fvalue = fvalue;
}
ConstantSymbolEntry::ConstantSymbolEntry(Type *type, long long llvalue): SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->llvalue = llvalue;
}
std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;

    if((this->getType())->isInt())
    {
    buffer << value;
    printf("lookINT!!!%d\n",value);
    
    }
    
    if((this->getType())->isFloat())
    {
    
    buffer << fvalue;
    //printf("look!!!%f\n",fvalue);

    }
    if((this->getType())->isLongLong())
    {
    buffer << llvalue;
    //printf("look!!!%lld\n",llvalue);
    }
    return buffer.str();

}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
}

std::string IdentifierSymbolEntry::toStr()
{
    return name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "t" << label;
    return buffer.str();
}

std::string FunctionSymbolEntry::toStr() 
{
    std::string str = name + "(";
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        str += paramTypes[i]->toStr();
        if (i < paramTypes.size() - 1) str += ", ";
    }//输出所有参数类型
    str += ") -> " + returnType->toStr();//返回类型
    return str;
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;


    // 创建标准库函数 putint(int) 并添加到符号表
    Type* intType = TypeSystem::intType; //commonInt是一个静态变量，不应该被修改
    // 此处应该新建一个intType，而TypeSystem::intType = &commonInt，IntType TypeSystem::commonInt = IntType(4);  则TypeSystem::intType就是一个IntType(4)
    Type* voidType =TypeSystem::voidType;  // 假设 VoidType 是 Type 的派生类
    std::vector<Type*> paramTypes = {intType};

    FunctionSymbolEntry* putintEntry = new FunctionSymbolEntry(voidType, paramTypes, "putint");
    install("putint", putintEntry);

    // 创建标准库函数 getint() 并添加到符号表

    FunctionSymbolEntry* getintEntry = new FunctionSymbolEntry(intType, {}, "getint");
    install("getint", getintEntry);

    // 创建标准库函数 putfloat(float) 并添加到符号表
    Type* floatType = TypeSystem::floatType; //commonFloat是一个静态变量，不应该被修改
    paramTypes = {floatType};
    FunctionSymbolEntry* putfloatEntry = new FunctionSymbolEntry(voidType, paramTypes, "putfloat");
    install("putfloat", putfloatEntry);

    // 创建标准库函数 getfloat() 并添加到符号表
    FunctionSymbolEntry* getfloatEntry = new FunctionSymbolEntry(floatType, {}, "getfloat");
    install("getfloat", getfloatEntry);

    // 创建标准库函数 getarray(int[])并添加到符号表
    Type* intArrayType = new IntArrayType(1);
    paramTypes = {intArrayType};
    FunctionSymbolEntry* getarrayEntry = new FunctionSymbolEntry(intType, paramTypes, "getarray");
    install("getarray", getarrayEntry);

    // 创建标准库函数 putarray(int[])并添加到符号表
    FunctionSymbolEntry* putarrayEntry = new FunctionSymbolEntry(voidType, paramTypes, "putarray");
    install("putarray", putarrayEntry);

    // 创建标准库函数  putch(int) 并添加到符号表(将整数参数的值作为 ASCII 码，输出该 ASCII 码对应的字符)
    paramTypes = {intType};
    FunctionSymbolEntry* putchEntry = new FunctionSymbolEntry(voidType, paramTypes, "putch");
    install("putch", putchEntry);

    // 创建标准库函数  getch() 并添加到符号表(返回一个整数，表示读入的字符的 ASCII 码)
    FunctionSymbolEntry* getchEntry = new FunctionSymbolEntry(intType, {}, "getch");
    install("getch", getchEntry);


}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    SymbolTable* current = this;
    while (current != nullptr) {
        auto it = current->symbolTable.find(name);
        if (it != current->symbolTable.end()) {
            return it->second;
        }
        current = current->prev;
    }
    return nullptr;
}
// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
