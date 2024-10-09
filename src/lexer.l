%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include "common.h"
    
}
%{
    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
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
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
/*  Your code here (lab3). */

%%
    /* rules section */
"int" {
    dump_tokens("INT\t%s\n", yytext);
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
    decimal = atoi(yytext);
    dump_tokens("DECIMAL\t%s\t%d\n", yytext, decimal);
}

{ID} {
    dump_tokens("ID\t%s\n", yytext);
}

{EOL} yylineno++;

{WHITE}

    /*  Your code here (lab3). */
%%
/* user code section */
