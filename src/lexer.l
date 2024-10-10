%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include "common.h"     //引入common.h头文件,含一个枚举类型dump_type_t
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
        #include <iostream>
        #include <map>
        #include <string>

        // 符号表项结构
        struct SymbolTableEntry {
            std::string type; // 标识符的类型，例如 int, float 等
            int lineNumber;   // 标识符出现的行号
            // 你可以根据需要添加更多字段
        };

        // 符号表类
        class SymbolTable {
        public:
            // 插入符号表项
            void insert(const std::string& identifier, const SymbolTableEntry& entry) {
                table[identifier] = entry;
            }

            // 查询符号表项
            SymbolTableEntry* lookup(const std::string& identifier) {
                auto it = table.find(identifier);
                if (it != table.end()) {
                    return &it->second;
                } else {
                    return nullptr;
                }
            }

        private:
            std::map<std::string, SymbolTableEntry> table;
        };
        SymbolTable symbolTable;
    /* Your code here, if desired (lab3). */
%}

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
/*  Your code here (lab3). */

%%  
    /* rules section */     //规则部分
"int" {
    dump_tokens("INT\t%s\n", yytext);//如果匹配到关键字 "int"，调用 dump_tokens() 输出 "INT"，并打印匹配的文本 yytext（当前的匹配字符串）。
}

"void" {
    dump_tokens("VOID\t%s\n", yytext);
}

"if" {
    dump_tokens("IF\t%s\n", yytext);
}

"else" {
    dump_tokens("ELSE\t%s\n", yytext);
}

"return" {
    dump_tokens("RETURN\t%s\n", yytext);
}

"=" {
    dump_tokens("ASSIGN\t%s\n", yytext);
}

"<" {
    dump_tokens("LESS\t%s\n", yytext);
}

"+" {
    dump_tokens("ADD\t%s\n", yytext);
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
    dump_tokens("SEMICOLON\t%s\n", yytext);
}

"(" {
    dump_tokens("LPAREN\t%s\n", yytext);
}

")" {
    dump_tokens("RPAREN\t%s\n", yytext);
}

"{" {
    dump_tokens("LBRACE\t%s\n", yytext);
}

"}" {
    dump_tokens("RBRACE\t%s\n", yytext);
}

{DECIMIAL} {
    int decimal;
    decimal = atoi(yytext); //将字符串转换为整数
    dump_tokens("DECIMAL\t%s\t%d\n", yytext, decimal);
}

{OCTAL} {
        int decimal;
        decimal = strtol(yytext, NULL, 8);  // 将八进制字符串转换为十进制
        dump_tokens("OCTAL\t%s\t%d\n", yytext, decimal);    // 输出十进制数
    }

    
{HEX} {
        int decimal;
        decimal = strtol(yytext, NULL, 16);  // 将十六进制字符串转换为十进制
        dump_tokens("HEX\t%s\t%d\n", yytext, decimal);      // 输出十进制数
    }
{FLOAT} {
    float value;
    value = strtof(yytext, NULL);  // 将浮点型字符串转换为 float 类型
    dump_tokens("FLOAT\t%s\t%f\n", yytext, value);  // 输出浮点数
    //dump_tokens("FLOAT\t%s\n", yytext);
}

{ID} {
    dump_tokens("ID\t%s\n", yytext);
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
