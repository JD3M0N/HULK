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

#ifndef YY_YY_SRC_PARSER_PARSER_TAB_HPP_INCLUDED
# define YY_YY_SRC_PARSER_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 30 "src/Parser/parser.y"

    struct ClassBody;   /* adelantos para parser.tab.hpp        */
    struct MemberDef;   /* MemberDef ya está en ast.hpp         */

#line 54 "src/Parser/parser.tab.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LET = 258,                     /* LET  */
    IN = 259,                      /* IN  */
    WHILE = 260,                   /* WHILE  */
    FOR = 261,                     /* FOR  */
    IDENT = 262,                   /* IDENT  */
    IF = 263,                      /* IF  */
    ELSE = 264,                    /* ELSE  */
    ELIF = 265,                    /* ELIF  */
    FUNCTION = 266,                /* FUNCTION  */
    ARROW = 267,                   /* ARROW  */
    ASSIGN = 268,                  /* ASSIGN  */
    ASSIGN_DESTRUCT = 269,         /* ASSIGN_DESTRUCT  */
    TRUE = 270,                    /* TRUE  */
    FALSE = 271,                   /* FALSE  */
    NUMBER = 272,                  /* NUMBER  */
    STRING = 273,                  /* STRING  */
    PLUS = 274,                    /* PLUS  */
    MINUS = 275,                   /* MINUS  */
    MULT = 276,                    /* MULT  */
    DIV = 277,                     /* DIV  */
    MOD = 278,                     /* MOD  */
    POW = 279,                     /* POW  */
    CONCAT = 280,                  /* CONCAT  */
    LE = 281,                      /* LE  */
    GE = 282,                      /* GE  */
    EQ = 283,                      /* EQ  */
    NEQ = 284,                     /* NEQ  */
    LESS_THAN = 285,               /* LESS_THAN  */
    GREATER_THAN = 286,            /* GREATER_THAN  */
    OR = 287,                      /* OR  */
    AND = 288,                     /* AND  */
    LPAREN = 289,                  /* LPAREN  */
    RPAREN = 290,                  /* RPAREN  */
    LBRACE = 291,                  /* LBRACE  */
    RBRACE = 292,                  /* RBRACE  */
    COMMA = 293,                   /* COMMA  */
    SEMICOLON = 294,               /* SEMICOLON  */
    TYPE = 295,                    /* TYPE  */
    INHERITS = 296,                /* INHERITS  */
    NEW = 297,                     /* NEW  */
    SELF = 298,                    /* SELF  */
    BASE = 299,                    /* BASE  */
    DOT = 300,                     /* DOT  */
    ELIF_CONTENT = 301,            /* ELIF_CONTENT  */
    UMINUS = 302,                  /* UMINUS  */
    LET_IN_PREC = 303,             /* LET_IN_PREC  */
    WHILE_PREC = 304,              /* WHILE_PREC  */
    FOR_PREC = 305                 /* FOR_PREC  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 35 "src/Parser/parser.y"

  char* str;
  Expr* expr;
  Stmt* stmt;
  Program* prog;
  std::vector<StmtPtr>* stmts;
  std::vector<ExprPtr>* expr_list;
  std::vector<std::string>* str_list;
  std::pair<std::string, Expr*>* binding;
  std::vector<std::pair<std::string, Expr*>>* bindings;
  ClassBody* class_body;
  MemberDef* member_def;

#line 135 "src/Parser/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_PARSER_TAB_HPP_INCLUDED  */
