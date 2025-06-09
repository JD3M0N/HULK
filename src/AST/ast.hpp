// ast.hpp
#pragma once

#ifndef AST_HPP
#define AST_HPP

#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include "../Errors/location.hpp"

// Forward declarations básicas
class IContext;

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

// *** NUEVAS FORWARD DECLARATIONS PARA CLASES ***
struct ClassDef;
struct AttrDef;
struct MethodDef;
struct NewExpr;
struct AttributeAccessExpr;
struct MethodCallExpr;
struct SelfExpr;

// Forward declaration para SemanticValidator 
class SemanticValidator;

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
    
    // *** NUEVOS MÉTODOS PARA EXPRESIONES ORIENTADAS A OBJETOS ***
    virtual void visit(NewExpr *expr) = 0;
    virtual void visit(AttributeAccessExpr *expr) = 0;
    virtual void visit(MethodCallExpr *expr) = 0;
    virtual void visit(SelfExpr *expr) = 0;
    
    virtual ~ExprVisitor() = default;
};

struct StmtVisitor
{
    virtual void visit(Program *) = 0;
    virtual void visit(ExprStmt *) = 0;
    virtual void visit(FunctionDecl *) = 0;
    
    // *** NUEVOS MÉTODOS PARA DECLARACIONES ORIENTADAS A OBJETOS ***
    virtual void visit(ClassDef *) = 0;
    virtual void visit(AttrDef *) = 0;
    virtual void visit(MethodDef *) = 0;
    
    virtual ~StmtVisitor() = default;
};

// Base class for all expression nodes
struct Expr
{
    Location location;

    // Constructor que recibe ubicación
    Expr(const Location &loc = Location()) : location(loc) {}

    virtual void accept(ExprVisitor *v) = 0;
    
    // Método virtual para validación semántica con logging
    virtual bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) = 0;
    
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
    
    // Método virtual para validación semántica de statements con logging
    virtual bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) = 0;
    
    virtual ~Stmt() = default;
};

using StmtPtr = std::unique_ptr<Stmt>;

// Incluir las definiciones completas DESPUÉS de las declaraciones forward
#include "../SemanticAnalysis/context.hpp"

// Forward declaration de SemanticValidator para evitar dependencia circular
// Las implementaciones de validate se harán inline después de incluir semantic_validator.hpp

// program: father of all the statements
struct Program : Stmt
{
    std::vector<StmtPtr> stmts;
    
    void accept(StmtVisitor *v) override
    {
        v->visit(this);
    }
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override
    {
        // Los literales no necesitan logging especial, siempre son válidos
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
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override
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
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
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
    
    // Declaración solamente - implementación al final del archivo
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Incluir el validador semántico DESPUÉS de todas las declaraciones
#include "../SemanticAnalysis/semantic_validator.hpp"

// IMPLEMENTACIONES INLINE de los métodos validate que usan SemanticValidator

inline bool Program::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("Programa");
    }
    
    bool result = true;
    // NO hacer break en el primer error - continuar validando todo
    for (auto &stmt : stmts) {
        if (!stmt->validate(context, validator)) {
            result = false;
            // Continúa validando el resto de statements en lugar de hacer break
        }
    }
    
    if (validator) {
        validator->logContextExit("Programa");
    }
    
    return result;
}

inline bool ExprStmt::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("ExprStmt");
    }
    
    bool result = expr->validate(context, validator);
    
    if (validator) {
        validator->logContextExit("ExprStmt");
    }
    
    return result;
}

inline bool UnaryExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("UnaryExpr");
    }
    
    bool result = operand->validate(context, validator);
    
    if (validator) {
        validator->logContextExit("UnaryExpr");
    }
    
    return result;
}

inline bool BinaryExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("BinaryExpr");
    }
    
    bool leftValid = left->validate(context, validator);
    bool rightValid = right->validate(context, validator);
    
    if (validator) {
        validator->logContextExit("BinaryExpr");
    }
    
    return leftValid && rightValid;
}

inline bool CallExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("CallExpr: " + callee);
    }
    
    // Verificar que la función esté definida con la cantidad correcta de argumentos
    bool functionExists = context->isDefined(callee, static_cast<int>(args.size()));
    
    if (validator) {
        validator->logFunctionCheck(callee, static_cast<int>(args.size()), functionExists);
    }
    
    // AGREGAR ESTE BLOQUE: Si la función no está definida, añadir el error
    if (!functionExists && validator) {
        validator->addError("Funcion no definida: " + callee + " con " + 
                           std::to_string(args.size()) + " argumentos", Location());
    }
    
    // Validar todos los argumentos
    bool allArgsValid = true;
    for (auto &arg : args) {
        if (!arg->validate(context, validator)) {
            allArgsValid = false;
        }
    }
    
    if (validator) {
        validator->logContextExit("CallExpr: " + callee);
    }
    
    return functionExists && allArgsValid;
}

inline bool VariableExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    bool isDefined = context->isDefined(name);
    
    if (validator) {
        validator->logVariableCheck(name, isDefined);
    }
    
    if (!isDefined && validator) {
        validator->addError("Variable no definida: " + name, Location());
    }
    
    return isDefined;
}

inline bool LetExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("LetExpr: " + name);
    }
    
    // Primero validar el inicializador en el contexto actual
    if (!initializer->validate(context, validator)) {
        if (validator) {
            validator->logContextExit("LetExpr: " + name);
        }
        return false;
    }
    
    // Crear un contexto hijo para el cuerpo donde la variable está definida
    auto innerContext = context->createChildContext();
    
    // Definir la variable en el contexto hijo
    bool defined = innerContext->define(name);
    if (validator) {
        validator->logVariableDefinition(name, defined, "LetExpr");
    }
    
    if (!defined) {
        if (validator) {
            validator->logContextExit("LetExpr: " + name);
        }
        return false; // Error si ya existe en este contexto
    }
    
    // Validar el cuerpo en el contexto donde la variable está disponible
    bool bodyValid = body->validate(innerContext, validator);
    
    if (validator) {
        validator->logContextExit("LetExpr: " + name);
    }
    
    return bodyValid;
}

inline bool AssignExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("AssignExpr: " + name);
    }
    
    // La variable debe estar previamente definida para poder asignarle
    bool isDefined = context->isDefined(name);
    if (validator) {
        validator->logVariableCheck(name, isDefined, "AssignExpr");
    }
    
    if (!isDefined) {
        if (validator) {
            validator->logContextExit("AssignExpr: " + name);
        }
        return false;
    }
    
    // El valor a asignar debe ser válido
    bool valueValid = value->validate(context, validator);
    
    if (validator) {
        validator->logContextExit("AssignExpr: " + name);
    }
    
    return valueValid;
}

inline bool FunctionDecl::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("FunctionDecl: " + name);
    }
    
    // *** CAMBIO PRINCIPAL: Definir la función PRIMERO para permitir recursión ***
    bool functionDefined = context->define(name, params);
    if (validator) {
        validator->logFunctionDefinition(name, params, functionDefined);
    }
    
    if (!functionDefined) {
        if (validator) {
            validator->logContextExit("FunctionDecl: " + name);
        }
        return false; // Error si la función ya existe con la misma aridad
    }
    
    // Crear un contexto hijo para el cuerpo de la función
    auto innerContext = context->createChildContext();
    
    // Definir todos los parámetros en el contexto hijo
    for (const auto &param : params) {
        bool paramDefined = innerContext->define(param);
        if (validator) {
            validator->logVariableDefinition(param, paramDefined, "Parametro de " + name);
        }
        
        if (!paramDefined) {
            if (validator) {
                validator->logContextExit("FunctionDecl: " + name);
            }
            return false; // Error si hay parámetros duplicados
        }
    }
    
    // Validar el cuerpo en el contexto con los parámetros definidos
    // AHORA la función ya está definida en el contexto padre, permitiendo recursión
    if (!body->validate(innerContext, validator)) {
        if (validator) {
            validator->logContextExit("FunctionDecl: " + name);
        }
        return false;
    }
    
    if (validator) {
        validator->logContextExit("FunctionDecl: " + name);
    }
    
    return true;
}

inline bool IfExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("IfExpr");
    }
    
    if (!condition->validate(context, validator)) {
        if (validator) {
            validator->logContextExit("IfExpr");
        }
        return false;
    }
    
    if (!thenBranch->validate(context, validator)) {
        if (validator) {
            validator->logContextExit("IfExpr");
        }
        return false;
    }
    
    // elseBranch puede ser nullptr en algunos casos
    if (elseBranch && !elseBranch->validate(context, validator)) {
        if (validator) {
            validator->logContextExit("IfExpr");
        }
        return false;
    }
    
    if (validator) {
        validator->logContextExit("IfExpr");
    }
    
    return true;
}

inline bool ExprBlock::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("ExprBlock");
    }
    
    // Crear un contexto hijo para el bloque
    auto blockContext = context->createChildContext();
    
    for (auto &stmt : stmts) {
        if (!stmt->validate(blockContext, validator)) {
            if (validator) {
                validator->logContextExit("ExprBlock");
            }
            return false;
        }
    }
    
    if (validator) {
        validator->logContextExit("ExprBlock");
    }
    
    return true;
}

inline bool WhileExpr::validate(std::shared_ptr<IContext> context, SemanticValidator* validator) {
    if (validator) {
        validator->logContextEnter("WhileExpr");
    }
    
    if (!condition->validate(context, validator)) {
        if (validator) {
            validator->logContextExit("WhileExpr");
        }
        return false;
    }
    
    bool bodyValid = body->validate(context, validator);
    
    if (validator) {
        validator->logContextExit("WhileExpr");
    }
    
    return bodyValid;
}

#include "class_nodes.hpp"

#endif // AST_HPP