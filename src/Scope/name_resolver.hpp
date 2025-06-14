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
        auto parentScope     = currentScope_;
        // Creamos el nuevo scope de la clase
        auto classScopePtr   = std::make_shared<SymScope>(parentScope);

        // Preparamos la info de clase
        SymbolInfo classInfo;
        classInfo.kind       = SymbolInfo::CLASS;
        classInfo.classScope = classScopePtr;

        // Declaramos la clase en el scope padre
        if (parentScope->existsInCurrent(c->name))
            throw std::runtime_error("Redeclaración de clase: " + c->name);
        parentScope->declare(c->name, classInfo);

        // A partir de aquí, usamos classScopePtr para atributos y métodos
        currentScope_ = classScopePtr;

        /* 1-d) registra atributos como VARIABLES */
        for (auto &attr : c->attributes)
        {
            if (currentScope_->existsInCurrent(attr.first))
                throw std::runtime_error("Atributo repetido: " + attr.first);
            currentScope_->declare(attr.first, {SymbolInfo::VARIABLE});
            attr.second->accept(this); // inicializador
        }

        /* 1-e) registra métodos y resuelve cuerpos */
        for (auto &mStmt : c->methods)
        {
            auto *m = dynamic_cast<FunctionDecl *>(mStmt.get());
            if (!m) continue;

            if (currentScope_->existsInCurrent(m->name))
                throw std::runtime_error("Miembro repetido: " + m->name);
            currentScope_->declare(m->name, {SymbolInfo::FUNCTION});

            auto saveScope = currentScope_;
            currentScope_  = std::make_shared<SymScope>(classScopePtr);

            currentScope_->declare("self", {SymbolInfo::VARIABLE});
            if (c->parent != "Object")
                currentScope_->declare("base", {SymbolInfo::VARIABLE});

            for (auto &p : m->params)
            {
                if (currentScope_->existsInCurrent(p))
                    throw std::runtime_error("Parámetro repetido: " + p);
                currentScope_->declare(p, {SymbolInfo::VARIABLE});
            }

            m->body->accept(this);
            currentScope_ = saveScope;
        }

        // Al final, restauramos el scope externo
        currentScope_ = parentScope;
    }

    /* 2)  new Tipo( … )  ---------------------------------------- */
    void visit(NewExpr *expr) override
    {
        currentScope_->lookup(expr->typeName); // debe existir como CLASS
        for (auto &a : expr->args)
            a->accept(this);
    }

    /* 3)  self  -------------------------------------------------- */
    void visit(SelfExpr *) override
    {
        currentScope_->lookup("self"); // error si no estamos en método
    }

    /* 4)  base  -------------------------------------------------- */
    void visit(BaseExpr *) override
    {
        currentScope_->lookup("base"); // declarado sólo en métodos que heredan
    }

    /* 5)  obj.miembro  ------------------------------------------ */
    void visit(MemberAccessExpr *expr) override
    {
        expr->object->accept(this); // resuelve el LHS
        /*  Versión simplificada: no comprobamos el miembro aquí   */
    }

    void visit(MemberAssignExpr *expr) override
    {
        expr->object->accept(this);
        // Verifica que expr->member exista en el scope del objeto si lo deseas:
        expr->value->accept(this);
    }

    /* 6) llamadas a método: receptor.metodo(arg1, arg2…) */
    void visit(MethodCallExpr *expr) override
    {
        // 1) resolver nombres dentro del receptor
        expr->receiver->accept(this);

        // 2) resolver cada argumento
        for (auto &arg : expr->args)
            arg->accept(this);

        // 3) Verificar que el método exista en el scope de la clase:
        //    – Primero obtener el tipo estático del receptor (pendiente de inferencia de tipos).
        //    – Luego hacer:
        //         auto ci = /* lookup del SymbolInfo de la clase */;
        //         ci.classScope->lookup(expr->methodName);
    }
};
