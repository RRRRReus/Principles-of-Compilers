#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include <vector> 

class SymbolEntry;

class Node//节点类
{
private:
    static int counter;//计数器
    int seq;//序号
public:
    Node();
    int getSeq() const {return seq;};   //返回节点的序号
    virtual void output(int level) = 0;//输出函数，纯虚函数，要求所有派生类实现该函数
};

class ExprNode : public Node//表达式节点类
{
protected:
    SymbolEntry *symbolEntry;   //符号表项
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    SymbolEntry* getSymbolEntry() const { return symbolEntry; } // 添加访问器方法
};

class BinaryExpr : public ExprNode//二元表达式类
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, AND, OR, LESS}; //枚举所有可能的二元运算符
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
};

class Constant : public ExprNode//常数类
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class Id : public ExprNode//标识符类
{
public:
    Id(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};
class ArrayIndex //数组索引类
{
private:
    
public:
    std::vector<ExprNode*> index;
    ArrayIndex(std::vector<ExprNode*> index) :  index(index) {};
    ArrayIndex() {};
    void output(int level);

};

class Array : public ExprNode//数组类
{
private:
    std::string name;
    Id *id;
    ArrayIndex *arrayIndex;
    public:
    Array( Id *id, ArrayIndex *arrayIndex) : ExprNode(id->getSymbolEntry()), id(id), arrayIndex(arrayIndex) {};
    void output(int level);
};
class FuncCall : public ExprNode // 函数调用类
{
private:
    Id *func;   // 函数名
    std::vector<ExprNode*> args;    // 函数参数（注意类型应为 表达式 ）
public:
    FuncCall(SymbolEntry *se, Id *func, std::vector<ExprNode*> args) : ExprNode(se), func(func), args(args) {};
    void output(int level);
};





class StmtNode : public Node//语句节点类
{};


class ExprStmt : public StmtNode 
{
private:
    ExprNode *expr; //表达式
public:
    ExprStmt(ExprNode *expr) : expr(expr) {};
    void output(int level);
};

class CompoundStmt : public StmtNode//复合语句类
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

class SeqNode : public StmtNode//序列语句类
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
};

class DeclStmt : public StmtNode//声明语句类
{
private:
    Id *id;
    Array *array;
    ExprNode *expr;
public:
    DeclStmt(Id *id, ExprNode *expr) : id(id), expr(expr) {};
    DeclStmt(Array *array) : array(array), expr(nullptr) {};
    DeclStmt(Id *id) : id(id), expr(nullptr) {};
    void output(int level);
};

class IfStmt : public StmtNode//if语句类
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

class IfElseStmt : public StmtNode//if-else语句类
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
};

class ReturnStmt : public StmtNode//返回语句类
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
};

class AssignStmt : public StmtNode//赋值语句类
{
private:
    ExprNode *lval; //左值
    ExprNode *expr; //右值
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class FunctionDef : public StmtNode//函数定义类（参数呢？）
{
private:
    SymbolEntry *se;    // 函数符号表项
    std::vector<Id*> params;    //  增加参数列表  //？？？？？？？？？？？？？？？？？Id还是DeclStmt
    StmtNode *stmt;    // 函数体
public:
     FunctionDef(SymbolEntry *se, std::vector<Id*> params, StmtNode *stmt) 
        : se(se), params(params), stmt(stmt) {};
    void output(int level);
};


class WhileStmt : public StmtNode // while语句类
{
private:
    ExprNode *cond;     // 条件
    StmtNode *body;     // 循环体
public:
    WhileStmt(ExprNode *cond, StmtNode *body) : cond(cond), body(body) {};
    void output(int level);
};

class EmptyStmt : public StmtNode // 空语句类
{
public:
    EmptyStmt() {};
    void output(int level);
};
class Ast//抽象语法树类
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
};

#endif
