#ifndef __AST_H__
#define __AST_H__

#include <fstream>

class SymbolEntry;

class Node//节点类
{
private:
    static int counter;//计数器
    int seq;//序号
public:
    Node();
    int getSeq() const {return seq;};
    virtual void output(int level) = 0;//输出函数
};

class ExprNode : public Node//表达式节点类
{
protected:
    SymbolEntry *symbolEntry;
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
};

class BinaryExpr : public ExprNode//二元表达式类
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, AND, OR, LESS};
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

class StmtNode : public Node//语句节点类
{};

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
public:
    DeclStmt(Id *id) : id(id){};
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
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class FunctionDef : public StmtNode//函数定义类
{
private:
    SymbolEntry *se;
    StmtNode *stmt;
public:
    FunctionDef(SymbolEntry *se, StmtNode *stmt) : se(se), stmt(stmt){};
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
