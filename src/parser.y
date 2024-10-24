%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    float floattype;
    std::vector<ExprNode*>* arglisttype; // 添加 arglisttype
    std::vector<Id*>* paramlisttype; // 添加 stmtlisttype
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token <floattype> FLOAT
%token IF ELSE
%token INT VOID
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON
%token ADD SUB OR AND LESS ASSIGN
%token RETURN
%token WHILE
%token COMMA


%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef WhileStmt FuncCallStmt EmptyStmt
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp FuncCall
%nterm <arglisttype> ArgList // 实参 声明 ArgList 的类型
%nterm <paramlisttype> FuncFParams FuncFParam // 形参 声明 ParamList 的类型
//%nterm <paramlisttype> ParamList // 形参 声明 ParamList 的类型
%nterm <type> Type  // 声明 值 的类型 int void

%precedence THEN
%precedence ELSE


%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;

// 语句序列
Stmts   
    : Stmt {$$=$1;} // 语句序列只有一个语句
    | Stmts Stmt{   // 语句序列有多个语句
        $$ = new SeqNode($1, $2);
    }
    ;


// 语句
Stmt    
    : AssignStmt {$$=$1;} 
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | WhileStmt {$$=$1;}
    | FuncCallStmt {$$=$1;}
    | EmptyStmt {$$=$1;}
    ;

// 空语句
EmptyStmt
    : SEMICOLON{
        $$ = new EmptyStmt();
    }
    ;
// while 语句    
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3, $5);
    }
    ;

// 函数调用语句（函数调用 + ）
FuncCallStmt
    : FuncCall SEMICOLON {
        $$ = new ExprStmt($1);
    }
    ;


// 函数调用
FuncCall
    : ID LPAREN RPAREN {
        SymbolEntry *se = identifiers->lookup($1);// 在符号表里查找函数名
        if (se == nullptr) {
            fprintf(stderr, "Function \"%s\" is undefined\n", $1);
            assert(se != nullptr);
        }
        $$ = new FuncCall(se, new Id(se), {});//new Id(se) 创建一个表示函数名的 Id 对象
    }
    | ID LPAREN ArgList RPAREN {
        SymbolEntry *se = identifiers->lookup($1);
        if (se == nullptr) {
            fprintf(stderr, "Function \"%s\" is undefined\n", $1);
            assert(se != nullptr);
        }
        $$ = new FuncCall(se, new Id(se), *$3); // 使用 *$3 解引用指针
        delete $3; // 释放 ArgList
    }
    ;


/* FuncDef
    : Type ID FuncDefRest{
        Type *funcType;
        funcType = new FunctionType($1, {});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    ;

FuncDefRest
    : LPAREN RPAREN BlockStmt {
        SymbolEntry *se = identifiers->lookup($-2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, {}, $3);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | LPAREN ParamList RPAREN BlockStmt {
        SymbolEntry *se = identifiers->lookup($4);
        assert(se != nullptr);
        $$ = new FunctionDef(se, *$2, $4);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    ; */



/* FuncDef
    : Type ID
    LPAREN RPAREN BlockStmt {

        Type *funcType;
        funcType = new FunctionType($1, {});

        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);//在符号表中安装一个符号
        identifiers = new SymbolTable(identifiers);//新建一个符号表（新作用域）（函数本身的符号表）


        //SymbolEntry *se_this = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, {}, $5); // 无参函数传入空参数列表
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    | Type ID
    LPAREN ParamList RPAREN BlockStmt { 

        Type *funcType;
        funcType = new FunctionType($1, {});//第二个参数还需要传入参数的类型
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());//getLevel()返回当前符号表的层次，直接赋值给scope
        identifiers->install($2, se);   //在符号表中安装一个符号
        //identifiers = new SymbolTable(identifiers);//新建一个符号表（新作用域）

         // 新建符号表，用于存储函数形参，这一步要在函数体之前
        SymbolTable *paramScope = new SymbolTable(identifiers);
        identifiers = paramScope;

        //SymbolEntry *se_this = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, *$4, $6); // 使用参数列表
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ; */

//函数定义
FuncDef
    : Type ID LPAREN RPAREN BlockStmt {
        // 无参数的函数定义
        Type *returnType = $1; // 获取函数的返回类型
        Type *funcType = new FunctionType(returnType, {}); // 创建无参数的函数类型
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel()); // 创建函数的符号表条目
        identifiers->install($2, se); // 将函数名插入符号表
        $$ = new FunctionDef(se, {}, $5); // 创建函数定义节点，函数体为
        delete []$2; // 释放 ID 字符串的内存
    }
    | Type ID LPAREN FuncFParams RPAREN BlockStmt {
        // 带参数的函数定义
        Type *returnType = $1; // 获取函数的返回类型
        std::vector<Type*> paramTypes; // 用于存储参数的类型
        for (auto param : *$4) {
            paramTypes.push_back(param->getType()); // 获取每个参数的类型
        }
        Type *funcType = new FunctionType(returnType, paramTypes); // 创建带参数的函数类型
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel()); // 创建函数的符号表条目
        identifiers->install($2, se); // 将函数名插入符号表
        $$ = new FunctionDef(se, $4, $6); //
        delete []$2; // 释放 ID 字符串的内存
    }
    ;



// 实参列表
ArgList   
    : Exp {
        $$ = new std::vector<ExprNode*>();
        $$->push_back($1);
    }
    | ArgList COMMA Exp {
        $$ = $1;    
        $$->push_back($3);
    }
    ;

//形参列表（多个形参）
FuncFParams
    : FuncFParam { 
        $$ = new std::vector<Id*>(); // 创建一个新的形参列表
        $$->push_back($1); // 将单个形参添加到列表中
    }
    | FuncFParams COMMA FuncFParam {
        $$ = $1; // 使用现有的形参列表
        $$->push_back($3); // 将下一个形参添加到列表中
    }
    ;

//单个形参（包含类型、标识符和可选数组部分）
FuncFParam
    : Type ID { 
        // 形参为标量
        Type *type = $1; // 获取形参的类型
        SymbolEntry *se = new IdentifierSymbolEntry(type, $2, identifiers->getLevel()); // 创建符号表条目
        identifiers->install($2, se); // 将符号表条目插入符号表
        $$ = new Id(se); // 创建新的 Id 对象表示形参
        delete []$2; // 释放 ID 字符串的内存
    }
    | Type ID '[' ']' { 
        // 形参为一维数组
        Type *baseType = $1; // 获取基础类型
        Type *arrayType = new ArrayType(baseType); // 创建一维数组类型
        SymbolEntry *se = new IdentifierSymbolEntry(arrayType, $2, identifiers->getLevel()); // 创建符号表条目
        identifiers->install($2, se); // 将符号表条目插入符号表
        $$ = new Id(se); // 创建新的 Id 对象表示形参
        delete []$2; // 释放 ID 字符串的内存
    }
    | Type ID '[' ']' '[' Exp ']' {
        // 形参为多维数组
        Type *baseType = $1; // 获取基础类型
        Type *arrayType = new ArrayType(baseType); // 创建第一维数组类型
        arrayType = new ArrayType(arrayType, $6->getValue()); // 创建第二维数组类型（使用表达式的值）
        SymbolEntry *se = new IdentifierSymbolEntry(arrayType, $2, identifiers->getLevel()); // 创建符号表条目
        identifiers->install($2, se); // 将符号表条目插入符号表
        $$ = new Id(se); // 创建新的 Id 对象表示形参
        delete []$2; // 释放 ID 字符串的内存
        delete $6; // 释放表达式对象
    }
    ;


/* // 形参列表(注意应新建一个符号表，再进行插入！！！)
ParamList 
    : Type ID {
        Type *type = $1;//获取类型
        SymbolEntry *se = new IdentifierSymbolEntry(type, $2, identifiers->getLevel());//生成新的符号表项
        //SymbolTable *top = new SymbolTable(identifiers);
        identifiers->install($2, se);
        $$ = new std::vector<Id*>();
        $$->push_back(new Id(se));
        delete []$2;
    }
    | ParamList COMMA Type ID {
        Type *type = $3;
        SymbolEntry *se = new IdentifierSymbolEntry(type, $4, identifiers->getLevel());
        identifiers->install($4, se);
        $$ = $1;
        $$->push_back(new Id(se));
        delete []$4;
    }
    ; */




LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;

// 赋值语句
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
// 复合语句
BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    |   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        RBRACE 
        {
            $$ = new CompoundStmt(new EmptyStmt());
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    | FLOAT {
        printf("1now is float%f\n", $1);
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::floatType, $1);
        //printf("2now is float%f\n", se->fvalue);
        $$ = new Constant(se);
    }
    ;
AddExp
    :
    PrimaryExp {$$ = $1;}
    |
    AddExp ADD PrimaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB PrimaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    ;
LAndExp
    :
    RelExp {$$ = $1;}
    |
    LAndExp AND RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
Type
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }

    ;
DeclStmt
    :
    Type ID SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }
    ;

%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
