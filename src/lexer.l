%option noyywrap
%option nounput
%option noinput
%option yylineno
%top{
    #include <stdarg.h>
    #include "common.h"     //引入common.h头文件,含一个枚举类型dump_type_t
    #include <map>
    #include <string>
    #include <vector>
    #include <iostream>

    #define YY_USER_ACTION {           \
    yycolno += yyleng;            \
}
    

}
%{
    int yycolno=0;
    bool CanBeSINGLE_LINE_BLOCK=false;//真项表示接下来有出现单行代码块的可能
    bool BeginFUNCTION=false;//真项表示这个(代表for的括号开始
    int PAREN_LEVEL=0;
    int comment_level = 0; // 用于跟踪多行注释的嵌套级别

    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;       // 定义一个 va_list 类型的变量，用于存储可变参数
        va_start(args, format);     // 初始化可变参数列表，`format` 是第一个参数
        if (dump_type == TOKENS)    // 如果 dump_type 为 TOKENS，则将可变参数列表中的参数按照 format 格式输出到 yyout
            vfprintf(yyout, format, args);
        va_end(args);
    
    
    }


    inline void token_out(const char* format)
    {
        

        dump_tokens("%-20s", format);
        dump_tokens("%-10s %-5d %-5d\n", yytext, yylineno, yycolno);
         
       

    }
             // 符号表项结构
        class SymbolTableEntry {
            public:
            std::string type; // 标识符的类型，例如 int, float 等
            int lineNumber;   // 标识符出现的行号
            // 你可以根据需要添加更多字段
        };

    // 符号表类
    class SymbolTable {
    private:
    std::vector<std::map<std::string, SymbolTableEntry*>> tables; // 符号表栈

    public:
        SymbolTable() {
            tables.push_back(std::map<std::string, SymbolTableEntry*>());
        }
        // 进入新的作用域
        void enterScope() {
            tables.push_back(std::map<std::string, SymbolTableEntry*>());
        }

        // 退出当前作用域
        void exitScope() {
            if (!tables.empty()) {
                // 释放当前作用域中的所有符号表项
                for (auto& entry : tables.back()) {
                    delete entry.second;
                }
                tables.pop_back();
            } else {
                std::cerr << "Error: No scope to exit" << std::endl;
            }
        }

        // 插入符号表项
        void insert(const std::string& identifier, SymbolTableEntry* entry) {
            if (!tables.empty()) {
                if(tables.back().find(identifier) != tables.back().end())
                {
                    fprintf(stderr, "Error: Duplicate identifier '%s' at line %d\n", identifier.c_str(), entry->lineNumber);
                    return;
                }
                tables.back()[identifier] = entry;
            } else {
                std::cerr << "Error: No scope to insert into" << std::endl;
            }
        }


        // 查询符号表项
        SymbolTableEntry* lookup(const std::string& identifier) {
            for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
                auto entryIt = it->find(identifier);
                if (entryIt != it->end()) {
                    return entryIt->second;
                }
            }
            return nullptr;
        }

    };

    // 创建一个全局符号表对象
    SymbolTable symbolTable;
    // 全局变量，用于区分当前是否在声明标识符
    bool isDeclaration = false;
    /* Your code here, if desired (lab3). */



%}

%s DEFVAR
%s TRY
%s FOR_PAREN
%s SINGLE_LINE_BLOCK
%x SINGLE_COMMENT
%x MULTI_COMMENT
%s FUNCTION

/* definitions section */

DECIMIAL [-]?([1-9][0-9]*|0)
OCTAL 0[0-7]+               
HEX 0[xX][0-9a-fA-F]+
FLOAT_DECIMAL [-]?(([1-9][0-9]*\.[0-9]*([eE][+-]?[0-9]+)?)|(0?\.[0-9]+([eE][+-]?[0-9]+)?))
FLOAT_OCTAL 0[0-7]+\.[0-7]*([eE][+-]?[0-9]+)?
FLOAT_HEX 0[xX][0-9a-fA-F]+\.[0-9a-fA-F]*([pP][+-]?[0-9]+)?
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)        
WHITE [\t ]  
comma ","
semicolon ";"
ANY .
IF_START "if"[[:space:]]*\(
WHILE_START "while"[[:space:]]*\(
FUNCTION_START [[:alpha:]_][[:alpha:][:digit:]_]*[[:space:]]*\(
    
/*  Your code here (lab3). */
%%  
  /* rules section */     //规则部分
 
<DEFVAR>"," {
    isDeclaration=true;
    token_out("COMMA");
    //dump_tokens("\t%d\t%d", yytext);

}

<DEFVAR>";" {
    token_out("SEMICOLON");
    isDeclaration=false;
    BEGIN INITIAL;
}



"int" {
    isDeclaration=true;
    //dump_tokens("INT\t%s\t%d\n", yytext,yylineno);//如果匹配到关键字 "int"，调用 dump_tokens() 输出 "INT"，并打印匹配的文本 yytext（当前的匹配字符串）。
    token_out("INT");
    BEGIN (DEFVAR);
}

"float" {
    isDeclaration=true;
    token_out("FLOAT");
    BEGIN (DEFVAR);
    
}


"void" {
    isDeclaration=true;
    BEGIN (DEFVAR);

    token_out("VOID");}

"const" {
    token_out("CONST");
}
"break" {
    token_out("BREAK");
}
"continue" {
    token_out("CONTINUE");
}

<FUNCTION>"(" {
    if(BeginFUNCTION)
    {
        symbolTable.enterScope(); // 进入新的作用域
        BeginFUNCTION=false;
    }
    PAREN_LEVEL++;
    token_out("FUNCTION_LPAREN");
    
    
}
<FUNCTION>")" {
        token_out("RPAREN");
        PAREN_LEVEL--;
        if(PAREN_LEVEL==0)
        CanBeSINGLE_LINE_BLOCK=true;
    }
<FUNCTION>"{" {
    token_out("FUNCTION_LPAREN");
    BEGIN(INITIAL);

    }
<SINGLE_LINE_BLOCK>";" {
    symbolTable.exitScope(); // 退出当前作用域
    BEGIN(INITIAL);
    token_out("SEMICOLON");
}
 {IF_START} {
    token_out("IF_START");
    //yyless(yyleng - 1);
}
 {WHILE_START} {
    token_out("WHILE_START");
    //yyless(yyleng - 1);

 }

 {FUNCTION_START} {
    token_out("FUNCTION_START");
    symbolTable.enterScope(); // 进入新的作用域
    BEGIN FUNCTION;
    //yyless(yyleng - 1);
}
 
"if" {
    token_out("IF");
}

"," {
    token_out("COMMA");
}

"=" {
    token_out("ASSIGN");
    
}

"<" {
    token_out("LESS");
    
}

">" {
    token_out("GREATER");
    
}

"<=" {
    token_out("LESS_EQUAL");
    
}

">=" {
    token_out("GREATER_EQUAL");
    
}

"==" {
    token_out("EQUAL");
    
}

"!=" {
    token_out("NOT_EQUAL");
    
}

"&&" {
    token_out("AND");
    
}

"||" {
    token_out("OR");
    
}

"!" {
    token_out("NOT");
    
}


"+" {
    token_out("ADD");
    
}

"-" {
        token_out("SUB");
}

"*" {
    token_out("MUL");
}

"/" {
    token_out("DIV");
}

"%" {
    token_out("MOD");
}

"[" {
    token_out("LBRACKET");
}
"]" {
    token_out("RBRACKET");
}

";" {
    token_out("SEMICOLON");
}

"(" {
    token_out("LPAREN");
}

")" {
    token_out("RPAREN");
}

<INITIAL>"{" {
    token_out("LBRACE");
    symbolTable.enterScope(); // 进入新的作用域
}
"}" {
    token_out("RBRACE");
    symbolTable.exitScope(); // 退出当前作用域
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext); //将字符串转换为整数
    dump_tokens("%-20s", "DECIMIAL");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20d\n", decimal);


}

{OCTAL} {
        int decimal;
        decimal = strtol(yytext, NULL, 8);  // 将八进制字符串转换为十进制
    dump_tokens("%-20s", "OCTAL");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20d\n", decimal);
    }

    
{HEX} {
        int decimal;
        decimal = strtol(yytext, NULL, 16);  // 将十六进制字符串转换为十进制
    dump_tokens("%-20s", "HEX");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20d\n", decimal);
    }


{FLOAT_DECIMAL} {
    float value;
    value = strtof(yytext, NULL);  // 将浮点型字符串转换为 float 类型
    dump_tokens("%-20s", "FLOAT_DECIMAL");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20.4f\n", value);
}

{FLOAT_OCTAL} {
     char* integer_part_str = strtok(yytext, ".");   // 分割整数部分
    char* fractional_part_str = strtok(NULL, ".");  // 分割小数部分

    // 转换整数部分
    int integer_part = strtol(integer_part_str, NULL, 8);

    // 转换小数部分
    float fractional_part = 0.0;
    if (fractional_part_str != NULL) {
        float base = 1.0 / 8.0;  // 八进制小数的基数
        for (int i = 0; fractional_part_str[i] != '\0'; ++i) {
            fractional_part += (fractional_part_str[i] - '0') * base;
            base /= 8.0;
        }
    }

    // 组合整数部分和小数部分
    float value = integer_part + fractional_part;
    dump_tokens("%-20s", "FLOAT_OCTAL");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20f\n", value);
}

{FLOAT_HEX} {
    float value;
    value = (float)strtod(yytext, NULL);  // 使用 strtod 将十六进制浮点数转换为十进制浮点数
    dump_tokens("%-20s", "FLOAT_HEX");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20f\n", value);
}



 {FUNCTION_START} {
    token_out("FUNCTION_START");
    symbolTable.enterScope(); // 进入新的作用域
    BEGIN FUNCTION;
    yyless(yyleng - 1);
}


{ID} {
    SymbolTableEntry* entry = nullptr;
    if (isDeclaration) {
        // 插入符号表项
        entry = new SymbolTableEntry{"identifier", yylineno};
        symbolTable.insert(yytext, entry);
        isDeclaration = false; // 重置声明标识符标志
    } else {
        // 查找符号表项
        entry = symbolTable.lookup(yytext);
        if (entry == nullptr) {
            fprintf(stderr, "Error: Undeclared identifier '%s' at line %d\n", yytext, yylineno);
        }
    }
 
    dump_tokens("%-20s", "ID");
    dump_tokens("%-10s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20p\n", entry);


    }

"//" {
    token_out("SINGLE_COMMENT_START");
    BEGIN SINGLE_COMMENT;
}

<SINGLE_COMMENT>.* {
    token_out("SINGLE_COMMENT");
    // 处理单行注释的内容
}

<SINGLE_COMMENT>\n {
    token_out("SINGLE_COMMENT_END");
    BEGIN INITIAL;
}

"/*" {

    token_out("MULTI_COMMENT_START");
    BEGIN MULTI_COMMENT;
}

<MULTI_COMMENT>"*/" {
        token_out("MULTI_COMMENT_END");
        BEGIN INITIAL;
    
}


<MULTI_COMMENT>[^*|\n]+ {
    // 处理没有结束符的情况，匹配直到遇到 *
    // 如果注释里有*呢
    token_out("MULTI_COMMENT");
}





<MULTI_COMMENT>\n {
    yycolno = 0;  // 遇到换行符，将列号重置为 0
}



{WHITE}
{}
    /*  Your code here (lab3). */

\n  { 
    yycolno = 0;  // 遇到换行符，将列号重置为 0
    //fprintf(stderr, "Newline detected. Reset column to: %d\n", yycolno);
}

%%

 /* user code section */

    
       