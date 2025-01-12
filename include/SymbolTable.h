#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include <string>
#include <map>
#include <vector>
class Type;
class Operand;

class SymbolEntry
{
private:
    int kind;
protected:
    enum {CONSTANT, VARIABLE, TEMPORARY};
    Type *type;

public:
    SymbolEntry(Type *type, int kind);
    virtual ~SymbolEntry() {};
    bool isConstant() const {return kind == CONSTANT;};//是否是常量
    bool isTemporary() const {return kind == TEMPORARY;};//是否是临时变量
    bool isVariable() const {return kind == VARIABLE;};//是否是变量
    Type* getType() {return type;};//返回类型
    void setType(Type *type) {this->type = type;};//设置类型
    virtual std::string toStr() = 0;
    // You can add any function you need here.
};


/*  
    Symbol entry for literal constant. Example:

    int a = 1;

    Compiler should create constant symbol entry for literal constant '1'.
*/
class ConstantSymbolEntry : public SymbolEntry
{
private:
    int value;
    float fvalue;
    long long llvalue;
    int *pvalue;//指向整数的指针
    float *pfvalue;//指向浮点数的指针
public:
    int ArrayDim;//数组维度
    int *ArrayDimSize;//数组各维度大小
    int size;//数组总大小
    void setIntValue(int value) {this->value = value;};
    ConstantSymbolEntry(int value);
    ConstantSymbolEntry(Type *type, int value);
    ConstantSymbolEntry(Type *type, float fvalue);
    ConstantSymbolEntry(Type *type, long long llvalue);
    ConstantSymbolEntry(Type *type, int *pvalue);
    ConstantSymbolEntry(Type *type, float* pfvalue);
    virtual ~ConstantSymbolEntry() {};
    int getValue() const {return value;};
    float getFloatValue() const {return fvalue;};
    void setFloatValue(float fvalue) {this->fvalue = fvalue;};
    std::string toStr();
    long long getLongLongValue() const {return llvalue;};
    // You can add any function you need here.
};


/* 
    Symbol entry for identifier. Example:

    int a;
    int b;
    void f(int c)
    {
        int d;
        {
            int e;
        }
    }

    Compiler should create identifier symbol entries for variables a, b, c, d and e:

    | variable | scope    |
    | a        | GLOBAL   |
    | b        | GLOBAL   |
    | c        | PARAM    |
    | d        | LOCAL    |
    | e        | LOCAL +1 |
*/
class IdentifierSymbolEntry : public SymbolEntry
{
private:
    enum {GLOBAL, PARAM, LOCAL};
    std::string name;
    int scope;  //作用域
    Operand *addr;  // The address of the identifier.
    // You can add any field you need here.
    std::string globalInitialValue; // 新增全局变量初始值

public:
    int ConstantValue;//如果是常量的话，整型常量值
    int ConstantFloatValue;//如果是常量的话，浮点常量值
    IdentifierSymbolEntry(Type *type, std::string name, int scope);//标识符的类型、名字、作用域
    IdentifierSymbolEntry(Type *type, std::string name, int scope,int ConstantValue);//作为一个const整形常量
    IdentifierSymbolEntry(Type *type, std::string name, int scope,float ConstantFloatValue);//作为一个const浮点常量
    virtual ~IdentifierSymbolEntry() {};
    std::string toStr();
    bool isGlobal() const {return scope == GLOBAL;};
    bool isParam() const {return scope == PARAM;};
    bool isLocal() const {return scope >= LOCAL;};//局部变量即位置大于等于LOCAL
    int getScope() const {return scope;};
    void setAddr(Operand *addr) {this->addr = addr;};
    Operand* getAddr() {return addr;};
    std::string getName() const {return name;};
    // You can add any function you need here.
    std::string getInitialValue() const { return globalInitialValue; } // 获取全局变量初始值
    void setInitialValue(const std::string &value) { 
        if(!value.empty()){
            globalInitialValue = value;
            }
            else{
                globalInitialValue = "0";//默认初始值为0！！！
            } } // 设置全局变量初始值
};


/* 
    Symbol entry for temporary variable created by compiler. Example:

    int a;
    a = 1 + 2 + 3;

    The compiler would generate intermediate code like:

    t1 = 1 + 2
    t2 = t1 + 3
    a = t2

    So compiler should create temporary symbol entries for t1 and t2:

    | temporary variable | label |
    | t1                 | 1     |
    | t2                 | 2     |
*/
class TemporarySymbolEntry : public SymbolEntry
{
private:
    int stack_offset;
    int label;
public:
    TemporarySymbolEntry(Type *type, int label);
    virtual ~TemporarySymbolEntry() {};
    std::string toStr();
    int getLabel() const {return label;};
    void setOffset(int offset) { this->stack_offset = offset; };
    int getOffset() { return this->stack_offset; };
    // You can add any function you need here.
};

// 用于库函数的 SymbolEntry 子类
class FunctionSymbolEntry : public SymbolEntry {
private:
    std::string name;
    std::vector<Type*> paramTypes;
    Type* returnType;

public:
    FunctionSymbolEntry(Type* funcType, Type* returnType, const std::vector<Type*>& paramTypes, const std::string& name)
        :SymbolEntry(funcType, VARIABLE), name(name), paramTypes(paramTypes), returnType(returnType) {}

    std::string getName() const { return name; }
    Type* getReturnType() const { return returnType; }
    const std::vector<Type*>& getParamTypes() const { return paramTypes; }
    
    std::string toStr() override;
};



// symbol table managing identifier symbol entries
class SymbolTable
{
private:
    std::map<std::string, SymbolEntry*> symbolTable;//符号表
    SymbolTable *prev;  //上一层符号表
    int level;  //当前符号表的层次
    static int counter; //计数器
public:
    SymbolTable();
    SymbolTable(SymbolTable *prev);
    void install(std::string name, SymbolEntry* entry);//安装符号表
    SymbolEntry* lookup(std::string name);//查找符号表
    SymbolEntry* lookupOnlyNow(std::string name);//只在当前符号表中查找
    SymbolTable* getPrev() {return prev;};
    int getLevel() {return level;};
    static int getLabel() {return counter++;};
};



extern SymbolTable *identifiers;    //标识符符号表，全局变量
extern SymbolTable *globals;    //全局符号表，全局变量

#endif
