#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type//类型基类
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC,FLOAT,INTARRAY,FLOATARRAY};
public:
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isFloat() const {return kind == FLOAT;};
    bool isIntArray() const {return kind == INTARRAY;};
    bool isFloatArray() const {return kind == FLOATARRAY;};


    
};

class IntType : public Type//整型类型
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    std::string toStr();
};

class ConstIntType : public IntType//常整型类型
{
public:
    ConstIntType() : IntType(4){};
    std::string toStr();
};
class FloatType : public Type//浮点类型
{
private:
    int size;
public:
    FloatType(int size) : Type(Type::FLOAT), size(size){};
    std::string toStr();
};
class ConstFloatType : public FloatType//常浮点类型
{
public:
    ConstFloatType() : FloatType(4){};
    std::string toStr();
};

class VoidType : public Type//空类型
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type//函数类型
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    void setParamsType(std::vector<Type*> paramTypes) {paramsType = paramTypes;}
    std::string toStr();
};

class IntArrayType : public Type//数组类型
{
private:
    int dim;
public:
    IntArrayType(int dim) : Type(Type::INTARRAY), dim(dim){};
    std::string toStr();
};
class FloatArrayType : public Type//数组类型
{
private:
    Type *baseType;
    int size;
public:
    FloatArrayType(Type *baseType, int size) : Type(Type::FLOATARRAY), baseType(baseType), size(size){};
    std::string toStr();
};

class TypeSystem//类型系统
{
private:
    static IntType commonInt;
    static VoidType commonVoid;
    static FloatType commonFloat;
    //static ConstIntType commonConstInt;
    //static ConstFloatType commonConstFloat;

public:
    static Type *intType;
    static Type *voidType;
    static Type *floatType;
    //static Type *constIntType;
    static Type *constFloatType;
};

#endif
