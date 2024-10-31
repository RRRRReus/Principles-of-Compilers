/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_HPP_INCLUDED
# define YY_YY_SRC_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 10 "src/parser.y"

    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"

#line 55 "src/parser.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    INTEGER = 259,                 /* INTEGER  */
    FLOAT = 260,                   /* FLOAT  */
    IF = 261,                      /* IF  */
    ELSE = 262,                    /* ELSE  */
    INT = 263,                     /* INT  */
    VOID = 264,                    /* VOID  */
    LPAREN = 265,                  /* LPAREN  */
    RPAREN = 266,                  /* RPAREN  */
    LBRACE = 267,                  /* LBRACE  */
    RBRACE = 268,                  /* RBRACE  */
    SEMICOLON = 269,               /* SEMICOLON  */
    LBRACKET = 270,                /* LBRACKET  */
    RBRACKET = 271,                /* RBRACKET  */
    ADD = 272,                     /* ADD  */
    SUB = 273,                     /* SUB  */
    MUL = 274,                     /* MUL  */
    DIV = 275,                     /* DIV  */
    MOD = 276,                     /* MOD  */
    OR = 277,                      /* OR  */
    AND = 278,                     /* AND  */
    ASSIGN = 279,                  /* ASSIGN  */
    LESS = 280,                    /* LESS  */
    LESSOREQUAL = 281,             /* LESSOREQUAL  */
    GREATER = 282,                 /* GREATER  */
    GREATEROREQUAL = 283,          /* GREATEROREQUAL  */
    EQUAL = 284,                   /* EQUAL  */
    NOTEQUAL = 285,                /* NOTEQUAL  */
    RETURN = 286,                  /* RETURN  */
    WHILE = 287,                   /* WHILE  */
    COMMA = 288,                   /* COMMA  */
    NOT = 289,                     /* NOT  */
    BREAK = 290,                   /* BREAK  */
    CONST = 291,                   /* CONST  */
    CONTINUE = 292,                /* CONTINUE  */
    THEN = 293                     /* THEN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "src/parser.y"

    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    float floattype;
    std::vector<ExprNode*>* arglisttype; // 添加 arglisttype
    std::vector<Id*>* paramlisttype; // 添加 stmtlisttype
    ArrayIndex* ArrayIndextype;

#line 122 "src/parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_HPP_INCLUDED  */
