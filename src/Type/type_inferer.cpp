#include "type_inferer.hpp"
#include <stdexcept>
#include <iostream>

#include "../Type/type.hpp" // ahora tienes sólo UNA versión de find/unify
#include "../AST/ast.hpp"

// -- Constructor: entorno global vacío --
TypeInfererVisitor::TypeInfererVisitor()
    : env(std::make_shared<Scope<TypePtr>>(nullptr))
{
    std::cout << "=== INICIALIZANDO INFERENCIA DE TIPOS ===" << std::endl;
    
    // NO registrar print aquí - será manejado en cada CallExpr
    std::cout << "Entorno base inicializado" << std::endl;
}

// -- ExprVisitor --
void TypeInfererVisitor::visit(NumberExpr *expr)
{
    expr->inferredType = Type::makeNumber();
}
void TypeInfererVisitor::visit(StringExpr *expr)
{
    expr->inferredType = Type::makeString();
}
void TypeInfererVisitor::visit(BooleanExpr *expr)
{
    expr->inferredType = Type::makeBoolean();
}
void TypeInfererVisitor::visit(UnaryExpr *expr)
{
    expr->operand->accept(this);
    expr->inferredType = expr->operand->inferredType;
}
void TypeInfererVisitor::visit(BinaryExpr *expr)
{
    expr->left->accept(this);
    expr->right->accept(this);
    auto L = expr->left->inferredType;
    auto R = expr->right->inferredType;

    using Op = BinaryExpr::Op;
    if (expr->op == Op::OP_ADD || expr->op == Op::OP_SUB ||
        expr->op == Op::OP_MUL || expr->op == Op::OP_DIV ||
        expr->op == Op::OP_POW || expr->op == Op::OP_MOD)
    {
        unify(L, R);
        expr->inferredType = Type::makeNumber();
    }
    else if (expr->op == Op::OP_AND || expr->op == Op::OP_OR)
    {
        unify(L, Type::makeBoolean());
        unify(R, Type::makeBoolean());
        expr->inferredType = Type::makeBoolean();
    }
    else if (expr->op == Op::OP_CONCAT)
    {
        unify(L, Type::makeString());
        unify(R, Type::makeString());
        expr->inferredType = Type::makeString();
    }
    else
    {
        // comparaciones -> Boolean
        unify(L, R);
        expr->inferredType = Type::makeBoolean();
    }
}
void TypeInfererVisitor::visit(CallExpr *expr)
{
    for (auto &arg : expr->args)
        arg->accept(this);

    TypePtr fnType;
    
    if(expr->callee == "print")
    {
        auto freshParam = Type::makeVar();
        auto freshRet = Type::makeVar();
        fnType = Type::makeFunction({freshParam}, freshRet);
    }
    else fnType = env->lookup(expr->callee); 
    
    for (size_t i = 0; i < expr->args.size(); ++i)
        unify(expr->args[i]->inferredType, fnType->params[i]);
    expr->inferredType = fnType->retType;
}
void TypeInfererVisitor::visit(VariableExpr *expr)
{
    expr->inferredType = env->lookup(expr->name);
}
void TypeInfererVisitor::visit(LetExpr *expr)
{
    expr->initializer->accept(this);
    auto initT = expr->initializer->inferredType;
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    env->declare(expr->name, initT);
    expr->body->accept(this);
    expr->inferredType = dynamic_cast<ExprStmt *>(expr->body.get())
                             ? static_cast<ExprStmt *>(expr->body.get())->expr->inferredType
                             : nullptr;
    env = parent;
}
void TypeInfererVisitor::visit(AssignExpr *expr)
{
    expr->value->accept(this);
    env->lookup(expr->name); // verificar existencia
    expr->inferredType = expr->value->inferredType;
}
void TypeInfererVisitor::visit(IfExpr *expr)
{
    expr->condition->accept(this);
    unify(expr->condition->inferredType, Type::makeBoolean());
    expr->thenBranch->accept(this);
    expr->elseBranch->accept(this);
    expr->inferredType = lub(
        expr->thenBranch->inferredType,
        expr->elseBranch->inferredType);
}
void TypeInfererVisitor::visit(ExprBlock *expr)
{
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    for (auto &s : expr->stmts)
        s->accept(this);
    // el tipo es el de la última expresión del bloque
    if (!expr->stmts.empty())
    {
        auto *last = dynamic_cast<ExprStmt *>(expr->stmts.back().get());
        expr->inferredType = last->expr->inferredType;
    }
    env = parent;
}
void TypeInfererVisitor::visit(WhileExpr *expr)
{
    expr->condition->accept(this);
    unify(expr->condition->inferredType, Type::makeBoolean());
    expr->body->accept(this);
    // devuelve el tipo de la última iteración
    auto *last = dynamic_cast<ExprStmt *>(expr->body.get());
    expr->inferredType = last ? last->expr->inferredType : Type::makeBoolean();
}

// -- StmtVisitor --
void TypeInfererVisitor::visit(ExprStmt *stmt)
{
    stmt->expr->accept(this);
}
void TypeInfererVisitor::visit(FunctionDecl *stmt)
{
    // Crear tipo función con variables de tipo para parámetros y retorno
    std::vector<TypePtr> ps;
    for (size_t i = 0; i < stmt->params.size(); ++i)
        ps.push_back(Type::makeVar());
    auto retV = Type::makeVar();
    auto fT = Type::makeFunction(ps, retV);
    env->declare(stmt->name, fT);

    // Nuevo scope para cuerpo
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    for (size_t i = 0; i < stmt->params.size(); ++i)
        env->declare(stmt->params[i], ps[i]);
    stmt->body->accept(this);
    // unificar retorno
    auto *eb = dynamic_cast<ExprStmt *>(stmt->body.get());
    unify(retV, eb->expr->inferredType);
    env = parent;
}

void TypeInfererVisitor::visit(Program *prog)
{
    // Simplemente recorremos cada statement del programa
    for (auto &stmt : prog->stmts)
    {
        stmt->accept(this);
    }
}
