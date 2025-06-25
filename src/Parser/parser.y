%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <memory>
#include "../AST/ast.hpp"

// Declarar yytext como externo
extern char* yytext;
extern int yylineno;

Program *rootAST;

using ExprPtr = std::unique_ptr<Expr>;
using ProgramPtr = std::unique_ptr<Program>;

using namespace std;
extern int yylex();
extern int yylineno;
void yyerror(const char* s);


struct ClassBody {
  std::vector<std::pair<std::string, ExprPtr>> attributes;
  std::vector<StmtPtr> methods;
};


%}

%code requires
{
    struct ClassBody;   /* adelantos para parser.tab.hpp        */
    struct MemberDef;   /* MemberDef ya está en ast.hpp         */
}

%union {
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
}

%start input

%type  <stmt> stmt
%type  <stmt> decl
%type  <expr> expr 
%type  <prog> input
%type  <prog> program
%type <stmts> stmt_list
%type <binding> binding
%type <str_list> ident_list
%type <bindings> binding_list 
%type <expr> if_expr elif_list
%type <expr_list> argument_list
%type <str> opt_inherits
%type <class_body> class_body
%type <member_def> member_def


%token LET IN 
%token WHILE FOR
%token <str> IDENT
%token IF ELSE ELIF
%token FUNCTION ARROW
%token ASSIGN ASSIGN_DESTRUCT
%token <expr> TRUE FALSE NUMBER STRING
%token PLUS MINUS MULT DIV MOD POW CONCAT
%token LE GE EQ NEQ LESS_THAN GREATER_THAN OR AND
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMICOLON
%token TYPE INHERITS NEW SELF BASE
%token DOT

// Agregar precedencias para resolver conflictos
%nonassoc ELIF_CONTENT
%right ASSIGN_DESTRUCT      // Asignación destructiva tiene baja precedencia
%right LET                  // LET tiene baja precedencia
%right WHILE FOR            // WHILE y FOR tienen baja precedencia  
%left OR
%left AND
%left EQ NEQ
%left LESS_THAN GREATER_THAN LE GE
%left PLUS MINUS
%left MULT DIV MOD
%left CONCAT
%right POW
%left DOT                   // DOT tiene alta precedencia (acceso a miembros)
%left UMINUS

// Agregar nuevos tokens de precedencia para las construcciones especiales:
%nonassoc LET_IN_PREC
%nonassoc WHILE_PREC
%nonassoc FOR_PREC

%%

input:
    program { rootAST = $1; }
  

program:
    /* vacío */            { $$ = new Program(); }

  | program stmt SEMICOLON {
        $1->stmts.emplace_back(StmtPtr($2));
        $$ = $1;
    }

  | program decl {
        $1->stmts.emplace_back(StmtPtr($2));
        $$ = $1;
    }
;

binding_list:
    binding {
          $$ = new std::vector<std::pair<std::string, Expr*>>();
          $$->push_back(*$1);
          delete $1;
      }
    | binding_list COMMA binding {
          $1->push_back(*$3);
          delete $3;
          $$ = $1;
      }
;

binding:
      IDENT ASSIGN expr {
          $$ = new std::pair<std::string, Expr*>(std::string($1), $3);
          free($1);
      }
;

decl:
    /* clase */
    TYPE IDENT opt_inherits LBRACE class_body RBRACE {
        ClassBody* cb = $5;
        $$ = new ClassDecl(
                 std::string($2),
                 std::string($3),
                 std::move(cb->attributes),
                 std::move(cb->methods)
             );
        delete cb;
        free($2);
        free($3);
    }

  | /* función full-form */
    FUNCTION IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE {
        auto args  = std::move(*$4);  delete $4;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$7); delete $7;

        $$ = new FunctionDecl(
                 std::string($2),
                 std::move(args),
                 std::move(block)
             );
        free($2);
    }
;

stmt:
    
   expr
    {
      $$ = new ExprStmt( ExprPtr($1) );
    }

  | FUNCTION IDENT LPAREN ident_list RPAREN ARROW expr  {
          std::vector<std::string> args = std::move(*$4);
          delete $4;

          $$ = new FunctionDecl(std::string($2), std::move(args), StmtPtr(new ExprStmt(ExprPtr($7))));
          free($2);
      }

;    

opt_inherits:
    /* vacío */            { 
        char* str = new char[strlen("Object") + 1];
        strcpy(str, "Object");
        $$ = str; 
    }
    | INHERITS IDENT        { $$ = $2; }
;

class_body:
      /* vacío */ {
          $$ = new ClassBody();
      }
    | class_body member_def {
          ClassBody* cb = $1;
          if ($2->isAttribute) {
              cb->attributes.push_back(std::move($2->attr));
          } else {
              cb->methods.push_back(std::move($2->method));
          }
          delete $2;
          $$ = cb;
    }
;

member_def:
    /* atributo */ IDENT ASSIGN expr SEMICOLON {
        $$ = new MemberDef{
            true,
            { std::string($1), ExprPtr($3) },      // <<< ExprPtr
            nullptr
        };
        free($1);
    }

  | /* método full-form */
    FUNCTION IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE {
        /* posiciones
            1: FUNCTION
            2: IDENT
            3: LPAREN
            4: ident_list   <<< argumentos
            5: RPAREN
            6: LBRACE
            7: stmt_list    <<< cuerpo
            8: RBRACE
        */
        auto args = std::move(*$4); delete $4;     // <<< $4, no $5
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*$7); delete $7;  // <<< $7, no $8

        FunctionDecl* fn = new FunctionDecl(
                               $2,
                               std::move(args),
                               std::move(block));
        $$ = new MemberDef{ false, {}, StmtPtr(fn) };
        free($2);
    }

  | /* método inline */
    FUNCTION IDENT LPAREN ident_list RPAREN ARROW expr {
        /* posiciones
            1: FUNCTION
            2: IDENT
            3: LPAREN
            4: ident_list   <<< argumentos
            5: RPAREN
            6: ARROW
            7: expr         <<< cuerpo
        */
        auto args = std::move(*$4); delete $4;     // <<< $4, no $5
        ExprStmt* body = new ExprStmt(ExprPtr($7));

        FunctionDecl* fn = new FunctionDecl(
                               $2,
                               std::move(args),
                               StmtPtr(body));
        $$ = new MemberDef{ false, {}, StmtPtr(fn) };
        free($2);
    }
;


stmt_list:
      /* vacío */ { $$ = new std::vector<StmtPtr>(); }
    | stmt_list stmt SEMICOLON {
        $1->emplace_back(StmtPtr($2));
        $$ = $1;
    }
;

ident_list:
      /* vacío */ { $$ = new std::vector<std::string>(); }
    | IDENT { $$ = new std::vector<std::string>(); $$->push_back($1); free($1); }
    | ident_list COMMA IDENT { $1->push_back($3); free($3); $$ = $1; }
;

expr:
      NUMBER            { $$ = $1; }
    | STRING            { $$ = $1; }
    | TRUE              { $$ = $1; }
    | FALSE             { $$ = $1; }

    | LBRACE stmt_list RBRACE {
          $$ = new ExprBlock(std::move(*$2));
          delete $2;
      }

    | IDENT LPAREN argument_list RPAREN {
          $$ = new CallExpr(std::string($1), std::move(*$3));
          delete $3;
          free($1);
      }

    | IDENT {
          $$ = new VariableExpr(std::string($1));
          free($1);
      }

    | MINUS expr %prec UMINUS {
          $$ = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr($2));
      }

    | expr POW expr {
          $$ = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr($1), ExprPtr($3));
      }

    | expr MULT expr {
          $$ = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr($1), ExprPtr($3));
      }

    | expr DIV expr {
          $$ = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr($1), ExprPtr($3));
      }

    | expr MOD expr %prec MULT {
          $$ = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr($1), ExprPtr($3));
      }

    | expr PLUS expr {
          $$ = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr($1), ExprPtr($3));
      }

    | expr MINUS expr {
          $$ = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr($1), ExprPtr($3));
      }

    | expr LESS_THAN expr {
          $$ = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr($1), ExprPtr($3));
      }

    | expr GREATER_THAN expr {
          $$ = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr($1), ExprPtr($3));
      }

    | expr LE expr {
          $$ = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr($1), ExprPtr($3));
      }

    | expr GE expr {
          $$ = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr($1), ExprPtr($3));
      }

    | expr EQ expr {
          $$ = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr($1), ExprPtr($3));
      }

    | expr NEQ expr {
          $$ = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr($1), ExprPtr($3));
      }

    | expr AND expr {
          $$ = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr($1), ExprPtr($3));
      }

    | expr OR expr {
          $$ = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr($1), ExprPtr($3));
      }

    | expr CONCAT expr {
        $$ = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr($1), ExprPtr($3));
    }  

    | LPAREN expr RPAREN {
          $$ = $2;
      }

    | LET binding_list IN expr %prec LET_IN_PREC {
          Expr* result = $4;
          auto& list = *$2;

          for (auto it = list.rbegin(); it != list.rend(); ++it) {
              result = new LetExpr(it->first, ExprPtr(it->second), std::make_unique<ExprStmt>(ExprPtr(result)));
          }

          delete $2;
          $$ = result;      
        }

    | IDENT ASSIGN_DESTRUCT expr {
          $$ = new AssignExpr(std::string($1), ExprPtr($3));
          free($1);
      }
    | WHILE LPAREN expr RPAREN expr %prec WHILE_PREC {
      $$ = new WhileExpr(ExprPtr($3), ExprPtr($5));
    }

    | if_expr  
    | FOR LPAREN IDENT IN expr RPAREN expr %prec FOR_PREC {
        auto argsNext = std::vector<ExprPtr>();
        argsNext.push_back(std::make_unique<VariableExpr>("__iter"));
        ExprPtr callNext = std::make_unique<CallExpr>("next", std::move(argsNext));

        auto argsCurrent = std::vector<ExprPtr>();
        argsCurrent.push_back(std::make_unique<VariableExpr>("__iter"));
        ExprPtr callCurrent = std::make_unique<CallExpr>("current", std::move(argsCurrent));

        ExprPtr bodyFor = ExprPtr($7);

        Expr* innerLetRaw = new LetExpr(
            std::string($3),                      // nombre de la variable (x)
            std::move(callCurrent),               // initializer = current(__iter)
            StmtPtr(new ExprStmt(std::move(bodyFor))) // cuerpo = ExprStmt(bodyFor)
        );

        Expr* whileRaw = new WhileExpr(
            std::move(callNext),
            ExprPtr(innerLetRaw)
        );

        auto argsIter = std::vector<ExprPtr>();
        argsIter.push_back( ExprPtr($5) );
        ExprPtr callIter = std::make_unique<CallExpr>("iter", std::move(argsIter));

        Expr* outerLetRaw = new LetExpr(
            "__iter",
            std::move(callIter),
            StmtPtr(new ExprStmt( ExprPtr(whileRaw) ))
        );

        free($3);  // liberar el IDENT
        $$ = outerLetRaw;
    }

    | NEW IDENT LPAREN argument_list RPAREN {
        $$ = new NewExpr(std::string($2), std::move(*$4));
        free($2); delete $4;
    }
    | SELF {
        $$ = new SelfExpr();
    }
    | BASE {
        $$ = new BaseExpr();
    }
    | expr DOT IDENT {
        $$ = new MemberAccessExpr( ExprPtr($1), std::string($3) );
        free($3);
    }  

;

if_expr:
    IF LPAREN expr RPAREN expr elif_list {
        $$ = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr($6));
    }
;

elif_list:
    ELSE expr %prec ELIF_CONTENT {
        $$ = $2;
    }
    | ELIF LPAREN expr RPAREN expr elif_list {
        $$ = new IfExpr(ExprPtr($3), ExprPtr($5), ExprPtr($6));
    }
;

argument_list:
      /* vacío */ { $$ = new std::vector<ExprPtr>(); }
    | expr {
          $$ = new std::vector<ExprPtr>();
          $$->emplace_back(ExprPtr($1));
      }
    | argument_list COMMA expr {
          $1->emplace_back(ExprPtr($3));
          $$ = $1;
      }
; 


%%

void yyerror(const char* s) {
    // Versión mejorada con más contexto
    std::string mensaje = std::string(s);
    std::cerr << "Error de sintaxis en linea " << yylineno << ": " << mensaje << std::endl;
    
    if (yytext[0] != '\0') {
        std::cerr << "Cerca del token: '" << yytext << "'" << std::endl;
    }
}