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


}
%{

    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;       // 定义一个 va_list 类型的变量，用于存储可变参数
        va_start(args, format);     // 初始化可变参数列表，`format` 是第一个参数
        if (dump_type == TOKENS)    // 如果 dump_type 为 TOKENS，则将可变参数列表中的参数按照 format 格式输出到 yyout
            vfprintf(yyout, format, args);
        va_end(args);
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
    bool StillDeclaration = false;
    /* Your code here, if desired (lab3). */
%}
%s DEFVAR
%s TRY
/* definitions section */
DECIMIAL ([1-9][0-9]*|0)
OCTAL 0[0-7]+               
HEX 0[xX][0-9a-fA-F]+
FLOAT ([0-9]+\.[0-9]*([eE][+-]?[0-9]+)?|[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?)   
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)        
WHITE [\t ]  
SINGLE_COMMENT "//".*(\r\n|\n|\r)
MULTI_COMMENT "/\*".*"\*/"
comma ","
semicolon ";"
/*  Your code here (lab3). */

%%  

    /* rules section */     //规则部分
<DEFVAR>"," {
    isDeclaration=true;
    dump_tokens("COMMA\t%s\t%d\n", yytext,yylineno);
}

<DEFVAR>";" {
    isDeclaration=false;
    BEGIN INITIAL;
}






"int" {
    isDeclaration=true;
    dump_tokens("INT\t%s\t%d\n", yytext,yylineno);//如果匹配到关键字 "int"，调用 dump_tokens() 输出 "INT"，并打印匹配的文本 yytext（当前的匹配字符串）。
    BEGIN (DEFVAR);
}

"void" {
    isDeclaration=true;
    BEGIN (DEFVAR);

    dump_tokens("VOID\t%s\t%d\n", yytext,yylineno);
}

"if" {
    dump_tokens("IF\t%s\t%d\n", yytext,yylineno);
}

"else" {
    dump_tokens("ELSE\t%s\t%d\n", yytext,yylineno);
}

"return" {
    dump_tokens("RETURN\t%s\t%d\n", yytext,yylineno);
}
"," {
    dump_tokens("COMMA\t%s\t%d\n", yytext,yylineno);
}

"=" {
    dump_tokens("ASSIGN\t%s\t%d\n", yytext,yylineno);
}

"<" {
    dump_tokens("LESS\t%s\t%d\n", yytext,yylineno);
}

"+" {
    dump_tokens("ADD\t%s\t%d\n", yytext,yylineno);
}

"-" {//????????????
    dump_tokens("SUB\t%s\n", yytext);
}

"*" {//????????????
    dump_tokens("MUL\t%s\n", yytext);
}

"/" {//???????
    dump_tokens("DIV\t%s\n", yytext);
}

";" {
    dump_tokens("SEMICOLON\t%s\t%d\n", yytext,yylineno);
}

"(" {
    dump_tokens("LPAREN\t%s\t%d\n", yytext,yylineno);
}

")" {
    dump_tokens("RPAREN\t%s\t%d\n", yytext,yylineno);
}

"{" {
    dump_tokens("LBRACE\t%s\t%d\n", yytext,yylineno);
    symbolTable.enterScope(); // 进入新的作用域
}

"}" {
    dump_tokens("RBRACE\t%s\t%d\n", yytext,yylineno);
    symbolTable.exitScope(); // 退出当前作用域
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext); //将字符串转换为整数
    dump_tokens("DECIMAL\t%s\t%d\t%d\n", yytext, yylineno,decimal);
}

{OCTAL} {
        int decimal;
        decimal = strtol(yytext, NULL, 8);  // 将八进制字符串转换为十进制
        dump_tokens("OCTAL\t%s\t%d\t%d\n", yytext, yylineno,decimal);    // 输出十进制数
    }

    
{HEX} {
        int decimal;
        decimal = strtol(yytext, NULL, 16);  // 将十六进制字符串转换为十进制
        dump_tokens("HEX\t%s\t%d\t%d\n", yytext, yylineno,decimal);      // 输出十进制数
    }
{FLOAT} {
    float value;
    value = strtof(yytext, NULL);  // 将浮点型字符串转换为 float 类型
    dump_tokens("FLOAT\t%s\t%d\t%d\n", yytext, yylineno,value);  // 输出浮点数
    //dump_tokens("FLOAT\t%s\n", yytext);
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
 
    dump_tokens("ID\t%s\t%d\t%p\n", yytext ,yylineno,entry);
}

{EOL} yylineno++;

{SINGLE_COMMENT} {
    dump_tokens("COMMENT\t%s\n", yytext);
}

{MULTI_COMMENT} {
    dump_tokens("COMMENT\t%s\n", yytext);
}

{WHITE}

    /*  Your code here (lab3). */
%%

 /* user code section */

    
       