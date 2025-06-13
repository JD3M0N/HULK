#pragma once
#include "../Type/type.hpp"
#include "../AST/ast.hpp"     // AST definitions :contentReference[oaicite:1]{index=1}
#include "../Scope/scope.hpp" // Scope para variables y funciones :contentReference[oaicite:2]{index=2}

class TypeInfererVisitor : public ExprVisitor, public StmtVisitor
{
    using TypePtr = std::shared_ptr<Type>;
    Scope<TypePtr>::Ptr env;

public:
    explicit TypeInfererVisitor();

    void visit(Program *prog);

    // ExprVisitor
    void visit(NumberExpr *expr) override;
    void visit(StringExpr *expr) override;
    void visit(BooleanExpr *expr) override;
    void visit(UnaryExpr *expr) override;
    void visit(BinaryExpr *expr) override;
    void visit(CallExpr *expr) override;
    void visit(VariableExpr *expr) override;
    void visit(LetExpr *expr) override;
    void visit(AssignExpr *expr) override;
    void visit(IfExpr *expr) override;
    void visit(ExprBlock *expr) override;
    void visit(WhileExpr *expr) override;
    void visit(NewExpr *expr) override;
    void visit(SelfExpr *expr) override;
    void visit(BaseExpr *expr) override;
    void visit(MemberAccessExpr *expr) override;
    void visit(MemberAssignExpr *expr) override;

    // StmtVisitor
    void visit(ExprStmt *stmt) override;
    void visit(FunctionDecl *stmt) override;
    void visit(ClassDecl *stmt) override;
};
