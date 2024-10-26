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
%token ADD SUB MUL DIV MOD OR AND ASSIGN LESS LESSOREQUAL GREATER GREATEROREQUAL EQUAL NOTEQUAL
%token RETURN
%token WHILE
%token COMMA
%token NOT // 添加 PLUS, MINUS, NOT 作为一元运算符

%token BREAK
%token CONST
%token CONTINUE

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef WhileStmt FuncCallStmt EmptyStmt ParamList Param funcStmt 
%nterm <stmttype> BreakStmt ContinueStmt
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp FuncCall Array InitVal UnaryExp MulExp EqExp
%nterm <arglisttype> ArgList // 实参 声明 ArgList 的类型
//%nterm <paramlisttype> FuncFParams FuncFParam // 形参 声明 ParamList 的类型
//%nterm <paramlisttype> ParamList // 形参 声明 ParamList 的类型
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
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    ;
BreakStmt
    : BREAK SEMICOLON{
        $$ = new BreakStmt();
    }
    ;
ContinueStmt
    : CONTINUE SEMICOLON{
        $$ = new ContinueStmt();
    }
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

/* //函数定义
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
    | Type ID LPAREN ParamList RPAREN BlockStmt {
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
    ; */

FuncDef
    :
    Type ID LPAREN {
        Type *funcType;
        funcType = new FunctionType($1,std::vector<Type*>());//必须先创建函数类型，这样才能创建函数符号表项
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    ParamList RPAREN funcStmt {

        std::vector<Type*> paramsType;
        DeclStmt* params = (DeclStmt*)$5;//参数列表
        //遍历所有参数，并且获取参数类型
        //如何获取参数类型：遍历所有的定义语句，找出Id，然后获取Id的符号表项，再获取此符号表项的类型
        while(params != nullptr)
        {
            paramsType.push_back(params->getId()->getSymbolEntry()->getType());
            params = (DeclStmt*)(params->getNext());
        }//获取所有的参数类型
        SymbolEntry *se;
        se = identifiers->lookup($2);// se为函数名的符号表项
        assert(se != nullptr);//断言函数名一定存在
        FunctionType* tmp = (FunctionType*)(se->getType());//(FunctionType*)将se->getType()转换为FunctionType*类型，se->getType()本身为Type*类型，因为FunctionType继承自Type
        tmp->setParamsType(paramsType);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!能否不要这个函数，直接在FunctionType的构造函数中传入参数类型
        $$ = new FunctionDef(se, (DeclStmt*)$5, new CompoundStmt($7));//se,参数列表，函数体(复合语句)
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();//返回上一层符号表
        delete top;
        delete []$2;
    }
    ;
funcStmt 
    : LBRACE Stmts RBRACE {
        $$ = $2;
    }
    | LBRACE RBRACE {//空函数
        $$ = new EmptyStmt();
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
    };
/* //形参列表（多个形参）
FuncFParams
    : FuncFParam { 
        $$ = new std::vector<Id*>(); // 创建一个新的形参列表
        $$->push_back($1); // 将单个形参添加到列表中
    }
    | FuncFParams COMMA FuncFParam {
        $$ = $1; // 使用现有的形参列表
        $$->push_back($3); // 将下一个形参添加到列表中
    }
    ; */

/* //单个形参（包含类型、标识符和可选数组部分）
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
    ; */


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


ParamList
    : %empty { $$ = nullptr; }
    | Param {
        $$ = $1;
    }
    | ParamList COMMA Param {
        $$ = $1;
        $$->addNodeList($3);//将下一个参数的指针赋值给当前参数的next
    }
    ;
Param
    : Type ID {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se));//创建一个声明语句
        delete []$2;
    }
    | Type ID ASSIGN Exp {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new DeclStmt(new Id(se), $4);
        delete []$2;
    }
    ;//是否添加数组类型的参数





// 左值
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
// 表达式
Exp
    :
    AddExp {$$ = $1;}
    | FuncCall {$$ = $1;} //???????????????????????？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
    ;
// 条件表达式
Cond
    :
    LOrExp {$$ = $1;}
    //| EqExp {$$ = $1;}
    ;
// 基本表达式(包含一个标识符一个常量)
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

// 一元运算符
/* UnaryOp
    : '+' { $$ = '+'; }
    | '-' { $$ = '-'; }
    ; */

//一元表达式 (函数调用、+、-、!，注：!仅出现在条件表达式中！！！怎么修改)
UnaryExp
    :PrimaryExp {$$ = $1;}
    | ADD UnaryExp {//PLUS是 加号 +
        SymbolEntry *se = $2->getSymbolEntry();
        $$ = new UnaryExpr(se, UnaryExpr::POS, $2);//需要新建一个一元表达式对象，一元表达式需要一个单独的类，因为和二元表达式的符号含义不同
    }
    | SUB UnaryExp {
        SymbolEntry *se = $2->getSymbolEntry();
        $$ = new UnaryExpr(se, UnaryExpr::NEG, $2);
    }
    | NOT UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    ;


//乘除模表达式
MulExp
    : UnaryExp { $$ = $1; }
    | MulExp MUL UnaryExp {
        if ($1->getSymbolEntry()->getType()->isFloat() || $3->getSymbolEntry()->getType()->isFloat()) {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
        } else {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
        }
    }
    | MulExp DIV UnaryExp {
        if ($1->getSymbolEntry()->getType()->isFloat() || $3->getSymbolEntry()->getType()->isFloat()) {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
        } else {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
        }
    }
    | MulExp MOD UnaryExp {
        if ($1->getSymbolEntry()->getType()->isFloat() || $3->getSymbolEntry()->getType()->isFloat()) {
            fprintf(stderr, "Error: float type can't use MOD operator\n");
            assert(false);
        } else {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
        }
    }
    ;


// 加减二元表达式
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        //PrimaryExp是一个左值/整数/浮点数，若为整数或浮点数，其是一个Exprnode子类Constant，需调用getSymbolEntry()访问其符号表项，再调用getType()访问其类型
        if($1->getSymbolEntry()->getType()->isFloat() || $3->getSymbolEntry()->getType()->isFloat())//注意，只要有一个是浮点数，结果就是浮点数！！！！
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());//处理整形变量
            $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);//接收四个变量，一个是符号表项，一个是运算符，操作数1，操作数2
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());//处理浮点变量
            $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);//接收四个变量，一个是符号表项，一个是运算符，操作数1，操作数2
        }
        
    }
    |
    AddExp SUB MulExp
    {
        if($1->getSymbolEntry()->getType()->isFloat() || $3->getSymbolEntry()->getType()->isFloat())
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
        }
        else
        {
            SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
        }
    }
    ;

// 逻辑表达式(目前只有整数！！！)
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp LESSOREQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSOREQUAL, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    |
    RelExp GREATEROREQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEROREQUAL, $1, $3);
    }
    ;


EqExp
    :
    RelExp {$$ = $1;}
    | EqExp EQUAL RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());//???????????????????什么意思
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    | EqExp NOTEQUAL RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    ;

LAndExp
    :
    EqExp {$$ = $1;}//相等性的优先级高于逻辑与
    |
    LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}//逻辑与的优先级高于逻辑或
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
    | CONST INT {
        IntType *intType = new IntType(4);
        intType->setConst(true);
        $$ = intType;
    }
    | CONST FLOAT {
        FloatType *floatType = new FloatType(32);
        floatType->setConst(true);
        $$ = floatType;
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
            intArrayType->setConst($1->getConst());
            se = new IdentifierSymbolEntry(intArrayType, $2, identifiers->getLevel());
        }
        else if($1->isFloat())
        {
            FloatArrayType *floatArrayType = new FloatArrayType(IndexDim.size());
            floatArrayType->setConst($1->getConst());
            se = new IdentifierSymbolEntry(floatArrayType, $2, identifiers->getLevel());
        }
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
