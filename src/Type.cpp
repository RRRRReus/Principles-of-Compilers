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
    std::ostringstream buffer;
    buffer << "i" << size;
    return buffer.str();
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr();
    return buffer.str();
}

std::string FloatType::toStr()
{
    if(getConst())
        return "float";
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

IntArrayType::IntArrayType(int dim):Type(Type::INTARRAY),dim(dim)
{
    fprintf(stderr,"IntArrayType::IntArrayType(int dim)函数被调用\n");
    dimSize = new std::vector<int>();
    //fprintf(stderr,"dimSize->size()是%ld\n",dimSize->size());
}
std::string IntArrayType::toStr()
{
    if(dimSize->size()==0)
    {
    
    std::ostringstream buffer;
    if(getConst())
        buffer << "const int[";
    else
        buffer << "i32[";
    for(int i = 0; i < dim; i++)
    {
        if(i != dim - 1)
            buffer << "][";
    }
    buffer << "]";
    return buffer.str();
    }
    else{

    std::ostringstream buffer;
    for(int i = 0; i < dim; i++)
    {
        buffer << "["<<(*dimSize)[i]<<" x ";
    }
    buffer << "i32";
    for(int i = 0; i < dim; i++)
    {
        buffer << "]";
    }
    return buffer.str();
        
    }

}
FloatArrayType::FloatArrayType(int dim):Type(Type::FLOATARRAY),dim(dim)
{
    this->dim = dim;
    dimSize = new std::vector<int>();
}
std::string FloatArrayType::toStr()
{
    if(dimSize->size()==0)
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
    else{

    std::ostringstream buffer;
    for(int i = 0; i < dim; i++)
    {
        buffer << "["<<(*dimSize)[i]<<" x ";
    }
    buffer << "float";
    for(int i = 0; i < dim; i++)
    {
        buffer << "]";
    }
    return buffer.str();
        
    }
}
std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}


std::string LongLongType::toStr()
{
    return "i32";
}

bool Type::isRetInt32()
{
    if(!this->isFunc())
        return false;
    else
    {
        if(dynamic_cast<FunctionType*>(this)->getRetType()->isInt())
            return true;
        else
            return false;
    }
}
