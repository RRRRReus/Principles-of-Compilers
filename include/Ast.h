#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Operand.h"

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node
{
private:
    static int counter;
    int seq;
protected:
    std::vector<BasicBlock**> true_list;//真出口
    std::vector<BasicBlock**> false_list;//假出口
    static IRBuilder *builder;
    /**
     * @brief 将列表中的所有指针指向target
     * @param list 待修改的列表
     * @param target 目标指针
     * 
     */
    void backPatch(std::vector<BasicBlock**> &list, BasicBlock*target);
    /**
     * @brief 合并两个列表
     * @param list1 第一个列表
     * @param list2 第二个列表
     * @return 合并后的列表
     * 
     */
    std::vector<BasicBlock**> merge(std::vector<BasicBlock**> &list1, std::vector<BasicBlock**> &list2);

public:
    Node();
    int getSeq() const {return seq;};   //返回节点的序号
    Node *next; //指向下一个节点
    Node* getNext() {return next;}
    void addNodeList(Node *n) {
        Node *p = this;
        while(p->next != nullptr) 
        {
            p = p->next;
        }
        p->next = n;//将n添加到链表的末尾
    }

    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<BasicBlock**>& trueList() {return true_list;}
    std::vector<BasicBlock**>& falseList() {return false_list;}
};

class ExprNode : public Node//表达式节点类
{
protected:
    SymbolEntry *symbolEntry;   //符号表项
    Operand *dst=nullptr;   // The result of the subtree is stored into dst. //存储子树的结果
public:
    bool CanBeCalculatedInt = false;//是否可以计算成一个整数
    int CalculatedInt;//如果可以计算成一个整数，那么这个整数是多少
    bool CanBeCalculatedFloat = false;//是否可以计算成一个浮点数
    float CalculatedFloat;//如果可以计算成一个浮点数，那么这个浮点数是多少
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    void setDst(Operand *op) {dst = op;};//设置操作数
    Operand* getOperand() {return dst;};//获取操作数
    SymbolEntry* getSymPtr() {return symbolEntry;};
    SymbolEntry* getSymbolEntry() const { return symbolEntry; } // 添加访问器方法
    void setSymbolEntry(SymbolEntry *se) { symbolEntry = se; } // 添加访问器方法
};

class BinaryExpr : public ExprNode//二元表达式类(所有二元运算符！！！)
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, MUL, DIV, MOD, AND, OR, LESS, LESSOREQUAL, GREATER, GREATEROREQUAL, EQUAL, NOTEQUAL}; //枚举所有可能的二元运算符
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){dst = new Operand(se);};
    // 公有的访问器方法
    ExprNode* getExpr1() const { return expr1; }
    ExprNode* getExpr2() const { return expr2; }
    void output(int level);
    void typeCheck();   //二元表达式类，需要进行类型检查
    void genCode();
};

// 一元表达式类
class UnaryExpr : public ExprNode
{   
private:
    int op;
    ExprNode *expr;
public:
    enum OpType { POS, NEG, NOT };
    UnaryExpr(SymbolEntry *se, int op, ExprNode *expr)
        : ExprNode(se), op(op), expr(expr) {}
    void output(int level);
    void typeCheck();
    void genCode();

};


class Constant : public ExprNode//常数类
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Id : public ExprNode//标识符类
{
public:
    Id(SymbolEntry *se) : ExprNode(se){SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ArrayIndex //数组索引类
{
private:
    
public:
    bool isVar = false;
    std::vector<ExprNode*> index;
    ArrayIndex(std::vector<ExprNode*> index) :  index(index) {};
    ArrayIndex() {};
    void output(int level);
    void typeCheck();
    void genCode();

};
class InitValList:public ExprNode//初始化值列表类
{
private:
public:
    std::vector<ExprNode*> initVal;

    std::string Dim1ToIR(int dim1);
    std::string Dim2ToIR(int dim1,int dim2);
    InitValList(std::vector<ExprNode*> initVal) : ExprNode(nullptr),initVal(initVal) {};
    void output(int level);
    void typeCheck();
    void genCode();
};
class Array : public ExprNode//数组类
{
private:
    std::string name;
    public:
    Id *id;
    Operand *element_addr=nullptr;
    ArrayIndex *arrayIndex;

    Array( Id *id, ArrayIndex *arrayIndex) : ExprNode(id->getSymbolEntry()), id(id), arrayIndex(arrayIndex) {};
    void output(int level);
    void typeCheck();
    void genCode();
};
class FuncCall : public ExprNode // 函数调用类
{
private:
    Id *func;   // 函数名
    std::vector<ExprNode*> args;    // 函数参数（注意类型应为 表达式 ）
public:
    FuncCall(SymbolEntry *se, Id *func, std::vector<ExprNode*> args) : ExprNode(se), func(func), args(args) {};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
};

class CompoundStmt : public StmtNode//复合语句类
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class SeqNode : public StmtNode//序列语句类
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclStmt : public StmtNode//声明语句类    标识符+表达式
{
private:
    Id *id;
    Array *array;
    ExprNode *expr;//表达式
    InitValList *initValList;//初始化值列表
public:
    DeclStmt(Id *id, ExprNode *expr) : id(id), expr(expr) {};
    DeclStmt(Array *array) : array(array), initValList(nullptr) {};
    DeclStmt(Id *id) : id(id), expr(nullptr) {};
    DeclStmt(Array *array, InitValList *initValList) : array(array), initValList(initValList) {};
    ExprNode *getId();  //返回标识符
    Array *getArray() { return array; }  //返回数组
    void output(int level);
    void typeCheck();
    void genCode();
};

class IfStmt : public StmtNode//if语句类
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
};

class ReturnStmt : public StmtNode//返回语句类
{
private:
    ExprNode *retValue;
public:
    ExprNode* getRetValue() {return retValue;}
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};
class BreakStmt : public StmtNode//break语句类
{
public:
    BreakStmt() {};
    void output(int level);
    void typeCheck();
    void genCode();

};
class ContinueStmt : public StmtNode//continue语句类
{
public:
    ContinueStmt() {};
    void output(int level);
    void typeCheck();
    void genCode();

    
};

class AssignStmt : public StmtNode//赋值语句类
{
private:
    ExprNode *lval; //左值
    ExprNode *expr; //右值
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FunctionDef : public StmtNode//函数定义类（参数呢？）
{
private:
    SymbolEntry *se;    // 函数符号表项
    //std::vector<Id*> params;    //  增加参数列表  //？？？？？？？？？？？？？？？？？Id还是DeclStmt
    DeclStmt *params;//通过链表将DeclStmt连在一起构成params（在语法分析阶段）
    StmtNode *stmt;    // 函数体
public:
     FunctionDef(SymbolEntry *se, DeclStmt *params, StmtNode *stmt) 
        : se(se), params(params), stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};


class WhileStmt : public StmtNode // while语句类
{
private:
    ExprNode *cond;     // 条件
    StmtNode *body;     // 循环体
public:
    WhileStmt(ExprNode *cond, StmtNode *body) : cond(cond), body(body) {};
    void output(int level);
    void typeCheck();
    void genCode();

};

class EmptyStmt : public StmtNode // 空语句类
{
public:
    EmptyStmt() {};
    void output(int level);
    void typeCheck();
    void genCode();

};
class Ast//抽象语法树类
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    void typeCheck();   //AST类，其typecheck就是调用root的typecheck，也就是Node的typecheck
    void genCode(Unit *unit);
};

#endif
