#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type//类型基类
{
private:
    int kind;
    bool isConst;
protected:
    
    enum {INT, VOID, FUNC,FLOAT,INTARRAY,FLOATARRAY,LONGLONG,PTR};
public:
    void setConst(bool isConst) {this->isConst = isConst;}
    bool getConst() {return isConst;}
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    int getKind(){return kind;}
    bool isInt() const {return kind == INT;};
    bool isRetInt32();
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isFloat() const {return kind == FLOAT;};
    bool isIntArray() const {return kind == INTARRAY;};
    bool isFloatArray() const {return kind == FLOATARRAY;};
    bool isLongLong() const {return kind == LONGLONG;};
    int getKind() const {return kind;};
    
};
class LongLongType : public Type//长整型类型
{
public:
    LongLongType() : Type(Type::LONGLONG){};
    std::string toStr();
};

class IntType : public Type//整型类型
{
private:
    int size;
public:
    int getSize(){return size;}
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
    Type* getRetType() {return returnType;};
    std::vector<Type*> getParamsType() {return paramsType;};//返回所有的参数类型
    void setParamsType(std::vector<Type*> paramTypes) {paramsType = paramTypes;}
    //void setParamsType_single(Type* paramType,int index) {paramsType[index] = paramType;}//用于在隐式转换时修改对应的参数类型
    std::string toStr();
};

class PointerType : public Type//指针类型
{
private:
    Type *valueType;
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};
class IntArrayType : public Type//数组类型
{
private:
    int dim;
public:
    int getDim(){return dim;}
    std::vector<int> *dimSize=nullptr;
    IntArrayType(int dim);
    std::string toStr();
};
class FloatArrayType : public Type//数组类型
{
private:
    int dim;
public:
    std::vector<int> *dimSize=nullptr;
    FloatArrayType(int dim);
    std::string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
    static FloatType commonFloat;
    static LongLongType commonLongLong;

public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
    static Type *floatType;
    static Type *longlongType;

};

#endif
