%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include <limits.h>
    #include <inttypes.h>
    #include "common.h"
    #include "parser.h"
}
%{
    int yycolno = 0;
    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
            vfprintf(yyout, format, args);
        va_end(args);
    }
    inline void token_out(const char* format)
    {
        

        dump_tokens("%-20s", format);
        dump_tokens("%-20s %-5d %-5d\n", yytext, yylineno, yycolno);
         
       

    }

    /* Your code here, if desired (lab1). */
%}
%x SINGLE_COMMENT
%x MULTI_COMMENT

/* definitions section */
DECIMIAL ([1-9][0-9]*|0)
OCTAL 0[0-7]+
HEX 0[xX][0-9a-fA-F]+ 
FLOAT_DECIMAL (([1-9][0-9]*\.[0-9]*([eE][+-]?[0-9]+)?)|(0?\.[0-9]+([eE][+-]?[0-9]+)?)|([1-9][0-9]*([eE][+-]?[0-9]+)))
FLOAT_HEX 0[xX]([0-9a-fA-F]+)?\.[0-9a-fA-F]*([pP][+-]?[0-9]+)?
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
/*  Your code here (lab1). */

%%
    /* rules section */
"int" {
    dump_tokens("INT\t%s\n", yytext);
    return INT;
}

"float" {
    dump_tokens("FLOAT\t%s\n", yytext);
    return FLOAT;
}

"void" {
    dump_tokens("VOID\t%s\n", yytext);
    return VOID;
}
"const" {
    dump_tokens("CONST\t%s\n", yytext);
    return CONST;
}
"break" {
    dump_tokens("BREAK\t%s\n", yytext);
    return BREAK;
}
"continue" {
    dump_tokens("CONTINUE\t%s\n", yytext);
    return CONTINUE;
}
"while" {
    dump_tokens("WHILE\t%s\n", yytext);
    return WHILE;
}

"if" {
    dump_tokens("IF\t%s\n", yytext);
    return IF;
}

"else" {
    dump_tokens("ELSE\t%s\n", yytext);
    return ELSE;
}

"return" {
    dump_tokens("RETURN\t%s\n", yytext);
    return RETURN;
}

"=" {
    dump_tokens("ASSIGN\t%s\n", yytext);
    return ASSIGN;
}

"<" {
    dump_tokens("LESS\t%s\n", yytext);
    return LESS;
}




"<=" {
    dump_tokens("LESSEQUAL\t%s\n", yytext);
    return LESSOREQUAL;
}

">" {
    dump_tokens("GREATER\t%s\n", yytext);
    return GREATER;
}

">=" {
    dump_tokens("GREATEREQUAL\t%s\n", yytext);
    return GREATEROREQUAL;
}

"&&" {
    dump_tokens("AND\t%s\n", yytext);
    return AND;
}

"||" {
    dump_tokens("OR\t%s\n", yytext);
    return OR;
}

"==" {
    dump_tokens("EQUAL\t%s\n", yytext);
    return EQUAL;
}

"!=" {
    dump_tokens("NOTEQUAL\t%s\n", yytext);
    return NOTEQUAL;
}

"!" {
    dump_tokens("NOT\t%s\n", yytext);
    return NOT;
}



"+" {
    dump_tokens("ADD\t%s\n", yytext);
    return ADD;
}

"-" {
    dump_tokens("SUB\t%s\n", yytext);
    return SUB;
}

"*" {
    dump_tokens("MUL\t%s\n", yytext);
    return MUL;
}

"/" {
    dump_tokens("DIV\t%s\n", yytext);
    return DIV;
}

"%" {
    dump_tokens("MOD\t%s\n", yytext);
    return MOD;
}

";" {
    dump_tokens("SEMICOLON\t%s\n", yytext);
    return SEMICOLON;
}

"(" {
    dump_tokens("LPAREN\t%s\n", yytext);
    return LPAREN;
}

")" {
    dump_tokens("RPAREN\t%s\n", yytext);
    return RPAREN;
}
"[" {
    dump_tokens("LBRACKET\t%s\n", yytext);
    return LBRACKET;
}
"]" {
    dump_tokens("RBRACKET\t%s\n", yytext);
    return RBRACKET;
}
"{" {
    dump_tokens("LBRACE\t%s\n", yytext);
    return LBRACE;
}

"}" {
    dump_tokens("RBRACE\t%s\n", yytext);
    return RBRACE;
}

"," {
    dump_tokens("COMMA\t%s\n", yytext);
    return COMMA;
}

{DECIMIAL} {
    int64_t decimal;
    decimal = strtoll(yytext,nullptr,10); //将字符串转换为longlong
    if(decimal > 0 && decimal > INT_MAX)
    {
        dump_tokens("%-20s", "LONGLONG");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20" PRId64 "\n", decimal);
        return LONGLONG;
    }
    else if(decimal<0&&decimal<INT_MIN)
    {
        dump_tokens("%-20s", "LONGLONG");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20" PRId64 "\n", decimal);
        yylval.ltype = decimal;
        return LONGLONG;
    }
    else{

        dump_tokens("%-20s", "DECIMIAL");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20d\n", (int)decimal);
        yylval.itype =  decimal;
        return INTEGER;

    }
}
{FLOAT_DECIMAL} {
    float value;
    value = strtof(yytext, NULL);  // 将浮点型字符串转换为 float 类型
    dump_tokens("%-20s", "FLOAT_DECIMAL");
    dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20f\n", value);
    yylval.floattype = value;
    return FLOAT;
}

{OCTAL} {
       int64_t decimal;
    decimal = strtoll(yytext,nullptr,8); //将字符串转换为longlong
    if(decimal > 0 && decimal > INT_MAX)
    {
        dump_tokens("%-20s", "LONGLONG");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20" PRId64 "\n", decimal);
        yylval.ltype = decimal;
        return LONGLONG;
    }
    else if(decimal<0&&decimal<INT_MIN)
    {
        dump_tokens("%-20s", "LONGLONG");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20" PRId64 "\n", decimal);
        yylval.ltype =  decimal;
        return LONGLONG;
    }
    else{

    dump_tokens("%-20s", "DECIMIAL");
    dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20d\n", (int)decimal);
    yylval.itype =  decimal;
    return INTEGER;

    }
}

{HEX} {
        int64_t decimal;
    decimal = strtoll(yytext,nullptr,16); //将字符串转换为longlong
    if(decimal > 0 && decimal > INT_MAX)
    {
        dump_tokens("%-20s", "LONGLONG");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20" PRId64 "\n", decimal);
        yylval.ltype =  decimal;
        return LONGLONG;
    }
    else if(decimal<0&&decimal<INT_MIN)
    {
        dump_tokens("%-20s", "LONGLONG");
        dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
        dump_tokens("%-20" PRId64 "\n", decimal);
        yylval.ltype =  decimal;
        return LONGLONG;
    }
    else{

    dump_tokens("%-20s", "DECIMIAL");
    dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20d\n", (int)decimal);
    yylval.itype =  decimal;
    return INTEGER;

    }
}

{FLOAT_HEX} {
    float value;
    value = (float)strtod(yytext, NULL);  // 使用 strtof 将十六进制浮点数转换为十进制浮点数
    dump_tokens("%-20s", "FLOAT_HEX");
    dump_tokens("%-20s %-5d %-5d ", yytext, yylineno, yycolno);
    dump_tokens("%-20f\n", value);
    yylval.floattype = value;
    return FLOAT;
}

{ID} {
    char *lexeme;
    dump_tokens("ID\t%s\n", yytext);
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}

{EOL} yylineno++;
"//" {
    //token_out("SINGLE_COMMENT_START");
    BEGIN SINGLE_COMMENT;
}

<SINGLE_COMMENT>.* {
    //token_out("SINGLE_COMMENT");
    // 处理单行注释的内容
}

<SINGLE_COMMENT>\n {
    //token_out("SINGLE_COMMENT_END");
    BEGIN INITIAL;
}

"/*" {

    //token_out("MULTI_COMMENT_START");
    BEGIN MULTI_COMMENT;
}

<MULTI_COMMENT>"*/" {
        //token_out("MULTI_COMMENT_END");
        BEGIN INITIAL;
    
}


<MULTI_COMMENT>[^*|\n]+ {
    // 处理没有结束符的情况，匹配直到遇到 *
    // 如果注释里有*呢
    //token_out("MULTI_COMMENT");
}





<MULTI_COMMENT>\n {
    yycolno = 0;  // 遇到换行符，将列号重置为 0
}



{WHITE}

    /*  Your code here (lab1). */
%%
/* user code section */
