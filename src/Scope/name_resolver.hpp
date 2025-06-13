#pragma once
#include <memory>
#include <stdexcept>
#include "scope.hpp"   // tu Scope<SymbolInfo> :contentReference[oaicite:1]{index=1}
#include "AST/ast.hpp" // nodos y visitor interfaces :contentReference[oaicite:2]{index=2}

class NameResolver : public StmtVisitor, public ExprVisitor
{
    using SymScope = Scope<SymbolInfo>;
    SymScope::Ptr currentScope_;

public:
    NameResolver()
        : currentScope_(std::make_shared<SymScope>(nullptr)) // scope global
    {
        // Pre-declarar las funciones nativas:
        for (auto &fn : {
                 "print", "sqrt", "log", "sin", "cos",
                 "rand", "range", "iter", "next", "current",
                 "PI", "E", "function", "if", "else"})
        {
            currentScope_->declare(fn, SymbolInfo{SymbolInfo::FUNCTION});
        }
    }

    // ---------------- StmtVisitor ----------------
    void visit(Program *p) override
    {
        for (auto &s : p->stmts)
            s->accept(this);
    }

    void visit(ExprStmt *s) override
    {
        // Una expresión suelta → resolver dentro de ella
        s->expr->accept(this);
    }

    void visit(FunctionDecl *f) override
    {
        // 1) Declarar la función
        if (currentScope_->existsInCurrent(f->name))
            throw std::runtime_error("Redeclaración de función: " + f->name);
        currentScope_->declare(f->name, {SymbolInfo::FUNCTION});
        // 2) Nuevo scope para parámetros + cuerpo
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        for (auto &param : f->params)
        {
            if (currentScope_->existsInCurrent(param))
                throw std::runtime_error("Redeclaración de parámetro: " + param);
            currentScope_->declare(param, {SymbolInfo::VARIABLE});
        }
        f->body->accept(this);
        // 3) Cerrar scope
        currentScope_ = parent;
    }

    // ---------------- ExprVisitor ----------------
    // 1) Literales no necesitan resolución
    void visit(NumberExpr *) override {}
    void visit(StringExpr *) override {}
    void visit(BooleanExpr *) override {}

    // 2) Operadores
    void visit(UnaryExpr *expr) override
    {
        expr->operand->accept(this);
    }
    void visit(BinaryExpr *expr) override
    {
        expr->left->accept(this);
        expr->right->accept(this);
    }

    // 3) Llamadas y variables
    void visit(CallExpr *expr) override
    {
        // Verifica que la función exista en el scope
        currentScope_->lookup(expr->callee);
        for (auto &arg : expr->args)
            arg->accept(this);
    }
    void visit(VariableExpr *expr) override
    {
        currentScope_->lookup(expr->name);
    }

    // 4) Let / Assign
    void visit(LetExpr *expr) override
    {
        expr->initializer->accept(this);
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        if (currentScope_->existsInCurrent(expr->name))
            throw std::runtime_error("Redeclaración de variable: " + expr->name);
        currentScope_->declare(expr->name, {SymbolInfo::VARIABLE});
        expr->body->accept(this);
        currentScope_ = parent;
    }
    void visit(AssignExpr *expr) override
    {
        // Verificar variable ya declarada
        currentScope_->lookup(expr->name);
        expr->value->accept(this);
    }

    // 5) Control de flujo
    void visit(IfExpr *expr) override
    {
        expr->condition->accept(this);
        expr->thenBranch->accept(this);
        if (expr->elseBranch)
            expr->elseBranch->accept(this);
    }
    void visit(ExprBlock *expr) override
    {
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        for (auto &stmt : expr->stmts)
            stmt->accept(this);
        currentScope_ = parent;
    }
    void visit(WhileExpr *expr) override
    {
        expr->condition->accept(this);
        auto parent = currentScope_;
        currentScope_ = std::make_shared<SymScope>(parent);
        expr->body->accept(this);
        currentScope_ = parent;
    }

    /* ----------  Soporte para orientación a objetos  ---------- */

    /* 1) Declaración de clase ---------------------------------- */
    void visit(ClassDecl *c) override
    {
        /* 1-a) declarar el nombre de la clase en el scope actual */
        if (currentScope_->existsInCurrent(c->name))
            throw std::runtime_error("Redeclaración de clase: " + c->name);
        currentScope_->declare(c->name, {SymbolInfo::CLASS});

        /* 1-b) si hereda, verificar que el padre exista y sea CLASS */
        if (c->parent != "Object")
            currentScope_->lookup(c->parent);  // —simple: asumimos que es CLASS—

        /* 1-c) nuevo scope “interno” para atributos/métodos        */
        auto outer = currentScope_;
        auto classScope = std::make_shared<SymScope>(outer);

        /* 1-d) registra atributos como VARIABLES                   */
        for (auto &attr : c->attributes)
        {
            if (classScope->existsInCurrent(attr.first))
                throw std::runtime_error("Atributo repetido: " + attr.first);
            classScope->declare(attr.first, {SymbolInfo::VARIABLE});
            attr.second->accept(this);          // inicializador
        }

        /* 1-e) registra métodos y resuelve cuerpos                 */
        for (auto &mStmt : c->methods)
        {
            auto *m = dynamic_cast<FunctionDecl *>(mStmt.get());
            if (!m) continue; // paranoico

            if (classScope->existsInCurrent(m->name))
                throw std::runtime_error("Miembro repetido: " + m->name);
            classScope->declare(m->name, {SymbolInfo::FUNCTION});

            /* scope propio del método ---------------------------- */
            auto save = currentScope_;
            currentScope_ = std::make_shared<SymScope>(classScope);

            /* insertar self (siempre) y base (si tiene padre) -----*/
            currentScope_->declare("self", {SymbolInfo::VARIABLE});
            if (c->parent != "Object")
                currentScope_->declare("base", {SymbolInfo::VARIABLE});

            /* parámetros */
            for (auto &p : m->params)
            {
                if (currentScope_->existsInCurrent(p))
                    throw std::runtime_error("Parámetro repetido: " + p);
                currentScope_->declare(p, {SymbolInfo::VARIABLE});
            }

            m->body->accept(this);
            currentScope_ = save;
        }

        /* no hace falta dejar classScope vivo — salimos — */
    }

    /* 2)  new Tipo( … )  ---------------------------------------- */
    void visit(NewExpr *expr) override
    {
        currentScope_->lookup(expr->typeName);   // debe existir como CLASS
        for (auto &a : expr->args)
            a->accept(this);
    }

    /* 3)  self  -------------------------------------------------- */
    void visit(SelfExpr *) override
    {
        currentScope_->lookup("self");           // error si no estamos en método
    }

    /* 4)  base  -------------------------------------------------- */
    void visit(BaseExpr *) override
    {
        currentScope_->lookup("base");           // declarado sólo en métodos que heredan
    }

    /* 5)  obj.miembro  ------------------------------------------ */
    void visit(MemberAccessExpr *expr) override
    {
        expr->object->accept(this);              // resuelve el LHS
        /*  Versión simplificada: no comprobamos el miembro aquí   */
    }

};
