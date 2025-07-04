// ast.hpp
#pragma once

#ifndef AST_HPP
#define AST_HPP

#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Type/type.hpp"

// Estructura simple para ubicación
struct Location
{
    int line = 0;
    int column = 0;

    Location() = default;
    Location(int l, int c) : line(l), column(c) {}

    std::string toString() const
    {
        return "line " + std::to_string(line) + ", column " + std::to_string(column);
    }
};

// Forward declarations
struct Program;
struct NumberExpr;
struct StringExpr;
struct BooleanExpr;
struct UnaryExpr;
struct BinaryExpr;
struct CallExpr;
struct VariableExpr;
struct LetExpr;
struct Stmt;
struct ExprStmt;
struct AssignExpr;
struct FunctionDecl;
struct IfExpr;
struct ExprBlock;
struct WhileExpr;

struct NewExpr;
struct BaseExpr;
struct SelfExpr;
struct MemberAccessExpr;
struct MemberAssignExpr;
struct MethodCallExpr;
struct ClassDecl;

// Visitors
struct ExprVisitor
{
    virtual void visit(Program *prog) = 0;
    virtual void visit(NumberExpr *expr) = 0;
    virtual void visit(StringExpr *expr) = 0;
    virtual void visit(BooleanExpr *) = 0;
    virtual void visit(UnaryExpr *expr) = 0;
    virtual void visit(BinaryExpr *expr) = 0;
    virtual void visit(CallExpr *expr) = 0;
    virtual void visit(VariableExpr *expr) = 0;
    virtual void visit(LetExpr *expr) = 0;
    virtual void visit(AssignExpr *expr) = 0;
    virtual void visit(IfExpr *) = 0;
    virtual void visit(ExprBlock *) = 0;
    virtual void visit(WhileExpr *) = 0;
    virtual void visit(NewExpr *expr) = 0;
    virtual void visit(SelfExpr *expr) = 0;
    virtual void visit(BaseExpr *expr) = 0;
    virtual void visit(MemberAccessExpr *expr) = 0;
    virtual void visit(MemberAssignExpr *expr) = 0;
    virtual ~ExprVisitor() = default;
};

struct StmtVisitor
{
    virtual void visit(Program *) = 0;
    virtual void visit(ExprStmt *) = 0;
    virtual void visit(FunctionDecl *) = 0;
    virtual void visit(ClassDecl *decl) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for all expression nodes
struct Expr
{
    Location location;

    Expr(const Location &loc = Location()) : location(loc) {}

    virtual void accept(ExprVisitor *v) = 0;
    std::shared_ptr<Type> inferredType;
    virtual ~Expr() = default;

    int getLine() const { return location.line; }
    int getColumn() const { return location.column; }
    const Location &getLocation() const { return location; }
};

using ExprPtr = std::unique_ptr<Expr>;

// base class for all statement nodes.
struct Stmt
{
    virtual void accept(StmtVisitor *) = 0;
    virtual ~Stmt() = default;
};

using StmtPtr = std::unique_ptr<Stmt>;

// program: father of all the statements
struct Program : Stmt
{
    std::vector<StmtPtr> stmts;
    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

// evaluates an expression
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(std::move(e)) {}
    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

// Literal: numeric
struct NumberExpr : Expr
{
    double value;
    NumberExpr(double v) : value(v) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Literal: string
struct StringExpr : Expr
{
    std::string value;
    StringExpr(const std::string &s) : value(s) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Literal: bool
struct BooleanExpr : Expr
{
    bool value;
    BooleanExpr(bool v) : value(v) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Unary operation: e.g., negation
struct UnaryExpr : Expr
{
    enum Op
    {
        OP_NEG
    } op;
    ExprPtr operand;
    UnaryExpr(Op o, ExprPtr expr) : op(o), operand(std::move(expr)) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Binary operation: +, -, *, /, ^, comparisons, mod
struct BinaryExpr : Expr
{
    enum Op
    {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_POW,
        OP_MOD,
        OP_LT,
        OP_GT,
        OP_LE,
        OP_GE,
        OP_EQ,
        OP_NEQ,
        OP_OR,
        OP_AND,
        OP_CONCAT
    } op;
    ExprPtr left;
    ExprPtr right;
    BinaryExpr(Op o, ExprPtr l, ExprPtr r) : op(o), left(std::move(l)), right(std::move(r)) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Function call: sqrt, sin, cos, exp, log, rand
struct CallExpr : Expr
{
    std::string callee;
    std::vector<ExprPtr> args;
    CallExpr(const std::string &name, std::vector<ExprPtr> &&arguments)
        : callee(name), args(std::move(arguments))
    {
    }
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// **VariableExpr**: para referirse a un identificador
struct VariableExpr : Expr
{
    std::string name;
    VariableExpr(const std::string &n) : name(n) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// **LetExpr**: let <name> = <init> in <body>
struct LetExpr : Expr
{
    std::string name;    // nombre de la variable
    ExprPtr initializer; // expresión inicializadora
    StmtPtr body;        // cuerpo donde la variable está en alcance
    LetExpr(const std::string &n, ExprPtr init, StmtPtr b)
        : name(n), initializer(std::move(init)), body(std::move(b))
    {
    }
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// a := b  destructive assignment
struct AssignExpr : Expr
{
    std::string name;
    ExprPtr value;

    AssignExpr(const std::string &n, ExprPtr v) : name(n), value(std::move(v)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// for function's declaration
struct FunctionDecl : Stmt
{
    std::string name;
    std::vector<std::string> params;
    StmtPtr body;

    FunctionDecl(const std::string &n, std::vector<std::string> &&p, StmtPtr b)
        : name(n), params(std::move(p)), body(std::move(b))
    {
    }

    void
    accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
};

// if-else expressions:
struct IfExpr : Expr
{
    ExprPtr condition;
    ExprPtr thenBranch;
    ExprPtr elseBranch;

    IfExpr(ExprPtr cond, ExprPtr thenB, ExprPtr elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB))
    {
    }

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// bloques de expresiones
struct ExprBlock : Expr
{
    std::vector<StmtPtr> stmts;
    ExprBlock(std::vector<StmtPtr> &&s) : stmts(std::move(s)) {}
    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// patra ciclos while
struct WhileExpr : Expr
{
    ExprPtr condition;
    ExprPtr body;

    WhileExpr(ExprPtr cond, ExprPtr b) : condition(std::move(cond)), body(std::move(b)) {}

    void
    accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
};

// Para distinguir atributos vs métodos en parser.y
struct MemberDef
{
    bool isAttribute;
    std::pair<std::string, ExprPtr> attr;
    StmtPtr method;
};

// Declaración de clase
struct ClassDecl : Stmt
{
    std::string name;
    std::string parent;
    std::vector<std::pair<std::string, ExprPtr>> attributes;
    std::vector<StmtPtr> methods;

    ClassDecl(std::string n, std::string p,
              std::vector<std::pair<std::string, ExprPtr>> attrs,
              std::vector<StmtPtr> m)
        : name(std::move(n)), parent(std::move(p)),
          attributes(std::move(attrs)), methods(std::move(m)) {}

    void accept(StmtVisitor *v) override { v->visit(this); }
};

// new Tipo(...)
struct NewExpr : Expr
{
    std::string typeName;
    std::vector<ExprPtr> args;
    NewExpr(std::string t, std::vector<ExprPtr> a)
        : typeName(std::move(t)), args(std::move(a)) {}
    void accept(ExprVisitor *v) override { v->visit(this); }
};

// self
struct SelfExpr : Expr
{
    void accept(ExprVisitor *v) override { v->visit(this); }
};

// base()
struct BaseExpr : Expr
{
    void accept(ExprVisitor *v) override { v->visit(this); }
};

// objeto.miembro
struct MemberAccessExpr : Expr
{
    ExprPtr object;
    std::string member;
    MemberAccessExpr(ExprPtr o, std::string m)
        : object(std::move(o)), member(std::move(m)) {}
    void accept(ExprVisitor *v) override { v->visit(this); }
};

// ast.hpp, tras MemberAccessExpr
struct MemberAssignExpr : Expr
{
    ExprPtr object;     // la expresión de objeto, ej. `self`
    std::string member; // el nombre del miembro, ej. "x"
    ExprPtr value;      // la expresión a asignar, ej. `x`
    MemberAssignExpr(ExprPtr o, std::string m, ExprPtr v)
        : object(std::move(o)),
          member(std::move(m)),
          value(std::move(v))
    {
    }
    void accept(ExprVisitor *v) override { v->visit(this); }
};

#endif // AST_HPP