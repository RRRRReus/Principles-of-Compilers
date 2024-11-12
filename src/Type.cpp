#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(32);
IntType TypeSystem::commonBool = IntType(1);
VoidType TypeSystem::commonVoid = VoidType();
FloatType TypeSystem::commonFloat = FloatType(32);
LongLongType TypeSystem::commonLongLong = LongLongType();

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::floatType= &commonFloat;
Type* TypeSystem::longlongType = &commonLongLong;


std::string IntType::toStr()
{
    //原来用的是buffer
    if(getConst())
        return "i32";
    else
        return "i32";
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "()";
    return buffer.str();
}

std::string FloatType::toStr()
{
    if(getConst())
        return "const void";
    else
        return "float";
}
std::string ConstIntType::toStr()
{
    return "const int";
}
std::string ConstFloatType::toStr()
{
    return "const float";
}

std::string IntArrayType::toStr()
{
    std::ostringstream buffer;
    if(getConst())
        buffer << "const int[";
    else
        buffer << "int[";
    for(int i = 0; i < dim; i++)
    {
        if(i != dim - 1)
            buffer << "][";
    }
    buffer << "]";
    return buffer.str();
}
std::string FloatArrayType::toStr()
{
    std::ostringstream buffer;
    if(getConst())
        buffer << "const float[";
    else
        buffer << "float[";
    for(int i = 0; i < dim; i++)
    {
        if(i != dim - 1)
            buffer << "][";
    }
    buffer << "]";
    return buffer.str();
}
std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}


std::string LongLongType::toStr()
{
    return "long long";
}
