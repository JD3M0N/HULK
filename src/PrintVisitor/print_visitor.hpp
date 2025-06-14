#ifndef PRINT_VISITOR_HPP
#define PRINT_VISITOR_HPP

#include <iostream>

#include "../AST/ast.hpp"

struct PrintVisitor : StmtVisitor, ExprVisitor
{
    int indentLevel = 0;

    void
    printIndent()
    {
        for (int i = 0; i < indentLevel; ++i)
            std::cout << "    "; // 4 espacios por nivel
    }

    // StmtVisitor:
    void
    visit(Program *p) override
    {
        std::cout << "Program\n";
        indentLevel++;
        for (auto &s : p->stmts)
            s->accept(this);
        indentLevel--;
    }

    void
    visit(ExprStmt *e) override
    {
        printIndent();
        std::cout << "|- ExprStmt\n";
        indentLevel++;
        e->expr->accept(this);
        indentLevel--;
    }

    // ExprVisitor
    void
    visit(NumberExpr *expr) override
    {
        printIndent();
        std::cout << "|_ " << expr->value << std::endl;
    }

    void
    visit(StringExpr *expr) override
    {
        printIndent();
        std::cout << "|_ \"" << expr->value << "\"" << std::endl;
    }

    void
    visit(BooleanExpr *expr) override
    {
        printIndent();
        std::cout << "|_ Boolean: " << (expr->value ? "true" : "false") << "\n";
    }

    void
    visit(UnaryExpr *expr) override
    {
        printIndent();
        std::cout << "|_ UnaryOp: -\n";
        indentLevel++;
        expr->operand->accept(this);
        indentLevel--;
    }

    void
    visit(BinaryExpr *expr) override
    {
        printIndent();
        std::cout << "|_ BinaryOp: " << opToString(expr->op) << "\n";
        indentLevel++;
        expr->left->accept(this);
        expr->right->accept(this);
        indentLevel--;
    }

    void
    visit(CallExpr *expr) override
    {
        printIndent();
        std::cout << "|_ Call: " << expr->callee << "\n";
        indentLevel++;
        for (const auto &arg : expr->args)
        {
            arg->accept(this);
        }
        indentLevel--;
    }

    void
    visit(VariableExpr *expr) override
    {
        printIndent();
        std::cout << "|_ Variable: " << expr->name << "\n";
    }

    void
    visit(LetExpr *expr) override
    {
        printIndent();
        std::cout << "|_ LetExpr: " << expr->name << "\n";
        indentLevel++;

        // 1) Mostrar subárbol del inicializador
        printIndent();
        std::cout << "|_ Initializer:\n";
        indentLevel++;
        expr->initializer->accept(this);
        indentLevel--;

        // 2) Mostrar subárbol del cuerpo
        printIndent();
        std::cout << "|_ Body:\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(AssignExpr *expr) override
    {
        printIndent();
        std::cout << "|_ AssignExpr: " << expr->name << "\n";
        indentLevel++;
        expr->value->accept(this);
        indentLevel--;
    }

    void
    visit(FunctionDecl *f) override
    {
        printIndent();
        std::cout << "|_FunctionDecl: " << f->name << "\n";
        indentLevel++;
        for (auto &p : f->params)
        {
            printIndent();
            std::cout << "|_ Param: " << p << "\n";
        }
        f->body->accept(this);
        indentLevel--;
    }

    void
    visit(IfExpr *expr) override
    {
        printIndent();
        std::cout << "|_ IfExpr\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Condition:\n";
        indentLevel++;
        expr->condition->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Then:\n";
        indentLevel++;
        expr->thenBranch->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Else:\n";
        indentLevel++;
        expr->elseBranch->accept(this);
        indentLevel--;

        indentLevel--;
    }

    void
    visit(ExprBlock *b) override
    {
        printIndent();
        std::cout << "|_ ExprBlock\n";
        indentLevel++;
        for (auto &stmt : b->stmts)
            stmt->accept(this);
        indentLevel--;
    }

    void
    visit(WhileExpr *expr) override
    {
        printIndent();
        std::cout << "|_ WhileExpr\n";
        indentLevel++;

        printIndent();
        std::cout << "|_ Condition:\n";
        indentLevel++;
        expr->condition->accept(this);
        indentLevel--;

        printIndent();
        std::cout << "|_ Body:\n";
        indentLevel++;
        expr->body->accept(this);
        indentLevel--;

        indentLevel--;
    }

    // === Soporte para clases y OO ===

    // Declaración de clase
    void visit(ClassDecl *c) override
    {
        printIndent();
        std::cout << "|_ ClassDecl: " << c->name;
        if (c->parent != "Object")
            std::cout << " inherits " << c->parent;
        std::cout << "\n";
        indentLevel++;

        // Atributos
        for (auto &attr : c->attributes)
        {
            printIndent();
            std::cout << "|_ Attribute: " << attr.first << "\n";
            indentLevel++;
            attr.second->accept(this);
            indentLevel--;
        }

        // Métodos
        for (auto &m : c->methods)
        {
            m->accept(this);
        }

        indentLevel--;
    }

    // new Tipo(arg1, arg2, …)
    void visit(NewExpr *expr) override
    {
        printIndent();
        std::cout << "|_ NewExpr: " << expr->typeName << "\n";
        indentLevel++;
        for (auto &arg : expr->args)
            arg->accept(this);
        indentLevel--;
    }

    // self
    void visit(SelfExpr * /*unused*/) override
    {
        printIndent();
        std::cout << "|_ SelfExpr\n";
    }

    // base
    void visit(BaseExpr * /*unused*/) override
    {
        printIndent();
        std::cout << "|_ BaseExpr\n";
    }

    // objeto.miembro
    void visit(MemberAccessExpr *expr) override
    {
        printIndent();
        std::cout << "|_ MemberAccess: " << expr->member << "\n";
        indentLevel++;
        expr->object->accept(this);
        indentLevel--;
    }
    void visit(MemberAssignExpr *expr) override
    {
        printIndent();
        std::cout << "|_ MemberAssign: " << expr->member << "\n";
        indentLevel++;
        expr->object->accept(this);
        expr->value->accept(this);
        indentLevel--;
    }

private:
    std::string
    opToString(BinaryExpr::Op op)
    {
        switch (op)
        {
        case BinaryExpr::OP_ADD:
            return "+";
        case BinaryExpr::OP_SUB:
            return "-";
        case BinaryExpr::OP_MUL:
            return "*";
        case BinaryExpr::OP_DIV:
            return "/";
        case BinaryExpr::OP_POW:
            return "^";
        case BinaryExpr::OP_MOD:
            return "%";
        case BinaryExpr::OP_LT:
            return "<";
        case BinaryExpr::OP_GT:
            return ">";
        case BinaryExpr::OP_LE:
            return "<=";
        case BinaryExpr::OP_GE:
            return ">=";
        case BinaryExpr::OP_EQ:
            return "==";
        case BinaryExpr::OP_NEQ:
            return "!=";
        case BinaryExpr::OP_OR:
            return "||";
        case BinaryExpr::OP_AND:
            return "&&";
        case BinaryExpr::OP_CONCAT:
            return "@";
        default:
            return "?";
        }
    }
};

#endif // PRINT_VISITOR_HPP