#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4);
VoidType TypeSystem::commonVoid = VoidType();
FloatType TypeSystem::commonFloat = FloatType(32);

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::floatType= &commonFloat;
// Type* TypeSystem::constIntType = &commonConstInt;
// Type* TypeSystem::constFloatType = &commonConstFloat;

std::string IntType::toStr()
{
    if(getConst())
        return "const int";
    else
        return "int";
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
    return "float[]";
}