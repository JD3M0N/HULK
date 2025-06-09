// ast.hpp
#pragma once

#ifndef AST_HPP
#define AST_HPP

#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include "../Errors/location.hpp"
#include "../SemanticAnalysis/context.hpp" 

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
    virtual ~ExprVisitor() = default;
};

struct StmtVisitor
{
    virtual void visit(Program *) = 0;
    virtual void visit(ExprStmt *) = 0;
    virtual void visit(FunctionDecl *) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for all expression nodes
struct Expr
{
    Location location;

    // Constructor que recibe ubicación
    Expr(const Location &loc = Location()) : location(loc) {}

    virtual void accept(ExprVisitor *v) = 0;
    
    // Método virtual para validación semántica
    virtual bool validate(std::shared_ptr<IContext> context) = 0;
    
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
    
    // Método virtual para validación semántica de statements
    virtual bool validate(std::shared_ptr<IContext> context) = 0;
    
    virtual ~Stmt() = default;
};

using StmtPtr = std::unique_ptr<Stmt>;

// program: father of all the statements
struct Program : Stmt
{
    std::vector<StmtPtr> stmts;
    
    void accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar todo el programa
    bool validate(std::shared_ptr<IContext> context) override
    {
        for (auto &stmt : stmts) {
            if (!stmt->validate(context)) {
                return false;
            }
        }
        return true;
    }
};

// evaluates an expression
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(std::move(e)) {}
    
    void accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar la expresión contenida
    bool validate(std::shared_ptr<IContext> context) override
    {
        return expr->validate(context);
    }
};

// Literal: numeric
struct NumberExpr : Expr
{
    double value;
    NumberExpr(double v) : value(v) {}
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Los literales numéricos siempre son válidos
    bool validate(std::shared_ptr<IContext> context) override
    {
        return true;
    }
};

// Literal: string
struct StringExpr : Expr
{
    std::string value;
    StringExpr(const std::string &s) : value(s) {}
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Los literales de string siempre son válidos
    bool validate(std::shared_ptr<IContext> context) override
    {
        return true;
    }
};

// Literal: bool
struct BooleanExpr : Expr
{
    bool value;
    BooleanExpr(bool v) : value(v) {}
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Los literales booleanos siempre son válidos
    bool validate(std::shared_ptr<IContext> context) override
    {
        return true;
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
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar el operando
    bool validate(std::shared_ptr<IContext> context) override
    {
        return operand->validate(context);
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
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar ambos operandos
    bool validate(std::shared_ptr<IContext> context) override
    {
        return left->validate(context) && right->validate(context);
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
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar que la función existe con la aridad correcta y que todos los argumentos sean válidos
    bool validate(std::shared_ptr<IContext> context) override
    {
        // Verificar que la función esté definida con la cantidad correcta de argumentos
        if (!context->isDefined(callee, static_cast<int>(args.size()))) {
            return false;
        }
        
        // Validar todos los argumentos
        for (auto &arg : args) {
            if (!arg->validate(context)) {
                return false;
            }
        }
        return true;
    }
};

// **VariableExpr**: para referirse a un identificador
struct VariableExpr : Expr
{
    std::string name;
    VariableExpr(const std::string &n) : name(n) {}
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar que la variable esté definida
    bool validate(std::shared_ptr<IContext> context) override
    {
        return context->isDefined(name);
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
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar let: primero el inicializador, luego definir la variable y validar el cuerpo
    bool validate(std::shared_ptr<IContext> context) override
    {
        // Primero validar el inicializador en el contexto actual
        if (!initializer->validate(context)) {
            return false;
        }
        
        // Crear un contexto hijo para el cuerpo donde la variable está definida
        auto innerContext = context->createChildContext();
        
        // Definir la variable en el contexto hijo
        if (!innerContext->define(name)) {
            return false; // Error si ya existe en este contexto
        }
        
        // Validar el cuerpo en el contexto donde la variable está disponible
        return body->validate(innerContext);
    }
};

// a := b  destructive assignment
struct AssignExpr : Expr
{
    std::string name;
    ExprPtr value;

    AssignExpr(const std::string &n, ExprPtr v) : name(n), value(std::move(v)) {}

    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar asignación: la variable debe existir y el valor debe ser válido
    bool validate(std::shared_ptr<IContext> context) override
    {
        // La variable debe estar previamente definida para poder asignarle
        if (!context->isDefined(name)) {
            return false;
        }
        
        // El valor a asignar debe ser válido
        return value->validate(context);
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

    void accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar declaración de función (siguiendo el patrón del texto)
    bool validate(std::shared_ptr<IContext> context) override
    {
        // Crear un contexto hijo para el cuerpo de la función
        auto innerContext = context->createChildContext();
        
        // Definir todos los parámetros en el contexto hijo
        for (const auto &param : params) {
            if (!innerContext->define(param)) {
                return false; // Error si hay parámetros duplicados
            }
        }
        
        // Validar el cuerpo en el contexto con los parámetros definidos
        if (!body->validate(innerContext)) {
            return false;
        }
        
        // Finalmente, definir la función en el contexto actual
        if (!context->define(name, params)) {
            return false; // Error si la función ya existe con la misma aridad
        }
        
        return true;
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

    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar if-else: condición y ambas ramas deben ser válidas
    bool validate(std::shared_ptr<IContext> context) override
    {
        if (!condition->validate(context)) {
            return false;
        }
        
        if (!thenBranch->validate(context)) {
            return false;
        }
        
        // elseBranch puede ser nullptr en algunos casos
        if (elseBranch && !elseBranch->validate(context)) {
            return false;
        }
        
        return true;
    }
};

// bloques de expresiones
struct ExprBlock : Expr
{
    std::vector<StmtPtr> stmts;
    ExprBlock(std::vector<StmtPtr> &&s) : stmts(std::move(s)) {}
    
    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar bloque: todos los statements deben ser válidos
    bool validate(std::shared_ptr<IContext> context) override
    {
        // Crear un contexto hijo para el bloque
        auto blockContext = context->createChildContext();
        
        for (auto &stmt : stmts) {
            if (!stmt->validate(blockContext)) {
                return false;
            }
        }
        return true;
    }
};

// para ciclos while
struct WhileExpr : Expr
{
    ExprPtr condition;
    ExprPtr body;

    WhileExpr(ExprPtr cond, ExprPtr b) : condition(std::move(cond)), body(std::move(b)) {}

    void accept(ExprVisitor *v) override
    {
        v->visit(this);
    }
    
    // Validar while: condición y cuerpo deben ser válidos
    bool validate(std::shared_ptr<IContext> context) override
    {
        if (!condition->validate(context)) {
            return false;
        }
        
        return body->validate(context);
    }
};

#endif // AST_HPP