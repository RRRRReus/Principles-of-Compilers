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
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
            vfprintf(yyout, format, args);
        va_end(args);
    }

    /* Your code here, if desired (lab3). */
%}

/* definitions section */
DECIMIAL ([1-9][0-9]*|0)
OCTAL 0[0-7]+               
HEX 0[xX][0-9a-fA-F]+        
ID [[:alpha:]_][[:alpha:][:digit:]_]*          
EOL (\r\n|\n|\r)        
WHITE [\t ]    
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

{ID} {
    dump_tokens("ID\t%s\n", yytext);
}

{EOL} yylineno++;   

{WHITE}

    /*  Your code here (lab3). */
%%
/* user code section */
