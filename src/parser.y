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
    ArrayIndex* ArrayIndextype;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token <floattype> FLOAT
%token IF ELSE
%token INT VOID
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET
%token ADD SUB OR AND LESS ASSIGN
%token RETURN
%token WHILE
%token COMMA


%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef WhileStmt FuncCallStmt EmptyStmt
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp FuncCall Array InitVal
%nterm <arglisttype> ArgList // 实参 声明 ArgList 的类型
%nterm <paramlisttype> ParamList // 形参 声明 ParamList 的类型
%nterm <type> Type  // 声明 值 的类型 int void
%nterm <ArrayIndextype> ArrayDim // 声明 数组 的类型
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



FuncDef
    : Type ID
    LPAREN RPAREN BlockStmt {

        Type *funcType;
        funcType = new FunctionType($1, {});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);


        //SymbolEntry *se = identifiers->lookup($2);
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
        funcType = new FunctionType($1, {});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);


       //SymbolEntry *se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, *$4, $6); // 使用参数列表
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;


// 实参列表
ArgList   
    : Exp {
        $$ = new std::vector<ExprNode*>();
        $$->push_back($1);
    }
    | ArgList COMMA Exp {
        $$ = $1;    // 使用 $1 而不是 $$->push_back($3);
        $$->push_back($3);
    }
    ;

ArrayDim
    :LBRACKET Exp RBRACKET{
        ArrayIndex *IndexDim = new ArrayIndex();
        IndexDim->index.push_back($2);
        $$ = IndexDim;
    }
    |ArrayDim LBRACKET Exp RBRACKET{
        $$ = $1;
        $$->index.push_back($3);
    }

Array
    : ID ArrayDim {
        SymbolEntry *se = identifiers->lookup($1);
        if (se == nullptr) {
            fprintf(stderr, "Array \"%s\" is undefined\n", $1);
            assert(se != nullptr);
        }
        ArrayIndex *IndexDim = $2;
        Id *name= new Id(se);
        $$ = new Array(name, IndexDim);
        

        delete []$1;
    }
    ;
// 形参列表
ParamList
    : Type ID {
        Type *type = $1;
        SymbolEntry *se = new IdentifierSymbolEntry(type, $2, identifiers->getLevel());
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
    ;


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
    | Array {
        $$ = $1;
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
        //printf("1now is float%f\n", $1);
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
    | FLOAT {
        $$ = TypeSystem::floatType;
    }

    ;
InitVal
    : Exp{
        $$ =$1;
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
    | Type ID ASSIGN InitVal SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se), $4);
        delete []$2;
    }
    | Type ID ArrayDim SEMICOLON {
        SymbolEntry *se;
        std::vector<ExprNode*> IndexDim= $3->index;
        if($1->isInt())
        {
            IntArrayType *intArrayType = new IntArrayType(IndexDim.size());
            se = new IdentifierSymbolEntry(intArrayType, $2, identifiers->getLevel());
        }
        // else if($1->getType() == TypeSystem::floatType)
        // {
        //     se = new IdentifierSymbolEntry(TypeSystem::intArrayType, $2, identifiers->getLevel());
        // }
        else
        {
            fprintf(stderr, "Error: unknown type\n");
            assert(false);
        }
        // se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        Id *name=new Id(se);
        identifiers->install($2, se);
        // std::vector<ExprNode*> IndexDim;
        // IndexDim.push_back($3);
        if (name == nullptr) {
    printf("Error: id is nullptr\n");
} else {
    printf("id is valid\n");
    if (name->getSymbolEntry() == nullptr) {
        printf("Error: id->getSymbolEntry() is nullptr\n");
    } else {
        printf("id->getSymbolEntry() is valid\n");
    }
}

        $$ = new DeclStmt(new Array(name, $3));
        
        printf("what?");
        delete []$2;
    }

%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
