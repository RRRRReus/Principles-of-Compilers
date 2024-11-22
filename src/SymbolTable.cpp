#include "SymbolTable.h"
#include "Type.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(int value): SymbolEntry(TypeSystem::intType,SymbolEntry::CONSTANT)
{
    this->value = value;
    this->type = TypeSystem::intType;
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
ConstantSymbolEntry::ConstantSymbolEntry(Type *type,int *pvalue): SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->pvalue = pvalue;
}
ConstantSymbolEntry::ConstantSymbolEntry(Type *type,float *pfvalue): SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->pfvalue = pfvalue;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;

    if((this->getType())->isInt())
    {
    buffer << value;
    //fprintf(stderr,"lookINT!!!%d\n",value);
    
    }
    
    if((this->getType())->isFloat())
    {
        fprintf(stderr,"!!!!!!!!!!!!一个常量float\n");
        fprintf(stderr, "floatValue: %f\n", fvalue);
         // 将 float 转换为 double，确保得到双精度值
        double doubleValue = double(fvalue);
        fprintf(stderr, "doubleValue: %f\n", doubleValue);

        // 解释为 IEEE 754 单精度浮点数
        uint64_t hexValue;
        std::memcpy(&hexValue, &doubleValue, sizeof(double)); // 将 double 转换为其二进制表示


        buffer << "0x" << std::hex << std::setw(16) << std::setfill('0') << hexValue;


    
          //buffer << fvalue;

    }
    if((this->getType())->isLongLong())
    {
    buffer << llvalue;
    //fprintf(stderr,"look!!!%lld\n",llvalue);
    }
    if((this->getType())->isIntArray())
    {
        for(int i = 0; i < size; i++)
        {
            buffer << pvalue[i] << " ";
        }
    }
    if((this->getType())->isFloatArray())
    {
        for(int i = 0; i < size; i++)
        {
            buffer << pfvalue[i] << " ";
        }
    }

    return buffer.str();

}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, int ConstantValue): SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
    this->ConstantValue = ConstantValue;
}
IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, float ConstantFloatValue): SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
    this->ConstantFloatValue = ConstantFloatValue;
}

std::string IdentifierSymbolEntry::toStr()
{
    return "@" + name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
}

std::string FunctionSymbolEntry::toStr() 
{
    // std::string str = name + "(";
    // for (size_t i = 0; i < paramTypes.size(); ++i) {
    //     str += paramTypes[i]->toStr();
    //     if (i < paramTypes.size() - 1) str += ", ";
    // }//输出所有参数类型
    // str += ") -> " + returnType->toStr();//返回类型
    // return str;
    return "@" + name;
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;


    // 创建标准库函数 putint(int) 并添加到符号表
    Type* intType = TypeSystem::intType; //commonInt是一个静态变量，不应该被修改(单例模型！！！！！！所有的intType都是指向commonInt的指针，都是一个对象)
    // 此处应该新建一个intType，而TypeSystem::intType = &commonInt，IntType TypeSystem::commonInt = IntType(4);  则TypeSystem::intType就是一个IntType(4)
    Type* voidType =TypeSystem::voidType;  // 假设 VoidType 是 Type 的派生类
    Type* floatType = TypeSystem::floatType; //commonFloat是一个静态变量，不应该被修改

    
    //创建void putint(int)函数
    std::vector<Type*> paramTypes = {intType};//参数类型
    FunctionType* putint_funcType = new FunctionType(voidType, paramTypes);//每一个库函数都具有一个自己的函数类型（构造函数参数：返回值类型、参数类型）
    FunctionSymbolEntry* putintEntry = new FunctionSymbolEntry(putint_funcType, voidType, paramTypes, "putint");
    install("putint", putintEntry);


    // 创建标准库函数 int getint() 并添加到符号表
    FunctionType* getint_funcType = new FunctionType(intType, {});
    FunctionSymbolEntry* getintEntry = new FunctionSymbolEntry(getint_funcType, intType, {}, "getint");
    install("getint", getintEntry);

    // 创建标准库函数 void putfloat(float) 并添加到符号表
    paramTypes = {floatType};
    FunctionType* putfloat_funcType = new FunctionType(voidType, paramTypes);//函数类型(构造函数传参：返回值类型，参数类型)
    FunctionSymbolEntry* putfloatEntry = new FunctionSymbolEntry(putfloat_funcType, voidType, paramTypes, "putfloat");
    install("putfloat", putfloatEntry);

    // 创建标准库函数 float getfloat() 并添加到符号表
    FunctionType* getfloat_funcType = new FunctionType(floatType, {});
    FunctionSymbolEntry* getfloatEntry = new FunctionSymbolEntry(getfloat_funcType, floatType, {}, "getfloat");
    install("getfloat", getfloatEntry);

    // 创建标准库函数 int getarray(int[])并添加到符号表
    Type* intArrayType = new IntArrayType(1);
    paramTypes = {intArrayType};
    FunctionType* getarray_funcType = new FunctionType(intType, paramTypes);
    FunctionSymbolEntry* getarrayEntry = new FunctionSymbolEntry(getarray_funcType, intType, paramTypes, "getarray");
    install("getarray", getarrayEntry);

    // 创建标准库函数 void putarray(int, int[])并添加到符号表
    paramTypes = {intType, intArrayType};
    FunctionType* putarray_funcType = new FunctionType(voidType, paramTypes);
    FunctionSymbolEntry* putarrayEntry = new FunctionSymbolEntry(putarray_funcType, voidType, paramTypes, "putarray");
    install("putarray", putarrayEntry);

    // 创建标准库函数 void putch(int) 并添加到符号表(将整数参数的值作为 ASCII 码，输出该 ASCII 码对应的字符)
    paramTypes = {intType};
    FunctionType* putch_funcType = new FunctionType(voidType, paramTypes);
    FunctionSymbolEntry* putchEntry = new FunctionSymbolEntry(putch_funcType, voidType, paramTypes, "putch");
    install("putch", putchEntry);

    // 创建标准库函数 int getch() 并添加到符号表(返回一个整数，表示读入的字符的 ASCII 码)
    FunctionType* getch_funcType = new FunctionType(intType, {});
    FunctionSymbolEntry* getchEntry = new FunctionSymbolEntry(getch_funcType, intType, {}, "getch");
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

SymbolEntry *SymbolTable::lookupOnlyNow(std::string name)
{
    SymbolTable* current = this;
        auto it = current->symbolTable.find(name);
        if (it != current->symbolTable.end()) {
            return it->second;
        }
    return nullptr;
}


