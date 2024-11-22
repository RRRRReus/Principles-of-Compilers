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
    bool isConstInt() const {return kind == INT && isConst;};
    bool isRetInt32();
    bool isRetFloat32();
    bool isAllInt() {return this->isInt() || this->isRetInt32();};//判断一个变量或者是函数是不是整型
    bool isAllFloat() {return this->isFloat() || this->isRetFloat32();};//判断一个变量或者是函数是不是浮点型
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isFuncVoid();
    bool isFloat() const {return kind == FLOAT;};
    bool isConstFloat() const {return kind == FLOAT && isConst;};
    bool isIntArray() const {return kind == INTARRAY;};
    bool isFloatArray() const {return kind == FLOATARRAY;};
    bool isLongLong() const {return kind == LONGLONG;};
    bool isPtr() const {return kind == PTR;};  
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
    Type* getValueType() {return valueType;}
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
    int getDim(){return dim;}
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
