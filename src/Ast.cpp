#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"
#include <cstdio>

extern FILE *yyout;
int Node::counter = 0;

Node::Node()
{
    seq = counter++;
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        //枚举变量在这派上用场了，枚举所有可能的二元运算符
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case MUL:
            op_str = "mul";
            break;
        case DIV:
            op_str = "div";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case LESS:
            op_str = "less";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}



void FuncCall::output(int level)    // 函数调用输出
{
    fprintf(yyout, "%*cFuncCall\tname: %s\n", level, ' ', func->getSymbolEntry()->toStr().c_str()); // 输出函数名
    for (auto arg : args)   // 遍历参数
    {
        arg->output(level + 4); // 输出参数
    }
}


void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    stmt1->output(level);
    stmt2->output(level);
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    if(array != nullptr)
        array->output(level + 4);
    if(id != nullptr)
    id->output(level + 4);
    if(expr != nullptr)
        expr->output(level + 4);
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr(); // se是函数符号表项，toStr()函数返回函数名
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    
    // 输出参数列表
    fprintf(yyout, "%*cParameters:\n", level + 4, ' ');

    while(params != nullptr)//遍历参数列表
        {
            std::string paramName = params->getId()->getSymbolEntry()->toStr();   // 通过参数获取参数名(必须先从DeclStmt提取出Id，才能获取其符号表项，进而输出)
            std::string paramType = params->getId()->getSymbolEntry()->getType()->toStr();// 通过参数获取参数类型
            int paramScope = dynamic_cast<IdentifierSymbolEntry*>(params->getId()->getSymbolEntry())->getScope(); // 获取参数的作用域
            fprintf(yyout, "%*c%s: %s, scope: %d\n", level + 8, ' ', paramName.c_str(), paramType.c_str(), paramScope); //输出参数名和参数类型
            params = (DeclStmt*)(params->getNext());
        }


    // for (auto param : params) {
    //     std::string paramName = param->getSymbolEntry()->toStr();   // 通过参数获取参数名
    //     std::string paramType = param->getSymbolEntry()->getType()->toStr();// 通过参数获取参数类型
    //     int paramScope = dynamic_cast<IdentifierSymbolEntry*>(param->getSymbolEntry())->getScope(); // 获取参数的作用域
    //     fprintf(yyout, "%*c%s: %s, scope: %d\n", level + 8, ' ', paramName.c_str(), paramType.c_str(), paramScope); //输出参数名和参数类型
    //     //fprintf(yyout, "%*c%s: %s\n", level + 8, ' ', paramName.c_str(), paramType.c_str());
    // }
    //获取函数参数的作用域并输出


    //scope = dynamic_cast<IdentifierSymbolEntry*>(se)->getScope();//获取函数参数的作用域
    
    // 输出函数体
    stmt->output(level + 4);
}


void WhileStmt::output(int level)   // While语句输出
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');   // 输出WhileStmt
    cond->output(level + 4);    // 输出条件
    body->output(level + 4);    // 输出循环体
}
void EmptyStmt::output(int level)   // 空语句输出
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');   // 输出EmptyStmt
}
void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

void Array::output(int level)
{
    fprintf(yyout, "%*cArray\n", level, ' ');
    id->output(level + 4);
    arrayIndex->output(level + 4);
    
}

void ArrayIndex::output(int level)
{
    fprintf(yyout, "%*cArrayIndex\n", level, ' ');
    int j=0;
    for(auto i : index)
    {
        fprintf(yyout, "%*cIndex", level + 4, ' ');
        fprintf(yyout, " %d:\n", j);
        i->output(level + 4);
        j++;
    }
}

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}
void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}