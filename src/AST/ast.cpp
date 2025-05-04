// ast.cpp
#include "ast.h"

namespace hulk
{

    ProgramNode::ProgramNode(
        std::vector<std::unique_ptr<DefinitionNode>> defs,
        std::unique_ptr<ExprNode> expr)
        : definitions(std::move(defs)), mainExpr(std::move(expr)) {}

    FuncDefNode::FuncDefNode(
        const std::string &n,
        std::vector<std::unique_ptr<ParamNode>> ps,
        std::unique_ptr<TypeNode> rt,
        std::unique_ptr<ExprNode> b)
        : name(n), params(std::move(ps)), returnType(std::move(rt)), body(std::move(b)) {}

    TypeDefNode::TypeDefNode(
        const std::string &n,
        std::unique_ptr<TypeNode> base,
        std::vector<std::unique_ptr<MemberNode>> m)
        : name(n), baseType(std::move(base)), members(std::move(m)) {}

    ProtoDefNode::ProtoDefNode(
        const std::string &n,
        std::vector<std::unique_ptr<MemberNode>> ms)
        : name(n), methods(std::move(ms)) {}

    ParamNode::ParamNode(
        const std::string &n,
        std::unique_ptr<TypeNode> t)
        : name(n), type(std::move(t)) {}

    TypeNode::TypeNode(const std::string &n)
        : name(n) {}

    AttrDefNode::AttrDefNode(
        const std::string &n,
        std::unique_ptr<TypeNode> t,
        std::unique_ptr<ExprNode> ie)
        : name(n), type(std::move(t)), initExpr(std::move(ie)) {}

    MethodDefNode::MethodDefNode(
        const std::string &n,
        std::vector<std::unique_ptr<ParamNode>> ps,
        std::unique_ptr<TypeNode> rt,
        std::unique_ptr<ExprNode> b)
        : name(n), params(std::move(ps)), returnType(std::move(rt)), body(std::move(b)) {}

    LiteralNode::LiteralNode(const std::string &v)
        : value(v) {}

    VarNode::VarNode(const std::string &n)
        : name(n) {}

    LetInNode::LetInNode(
        std::vector<std::unique_ptr<ParamNode>> v,
        std::unique_ptr<ExprNode> e)
        : vars(std::move(v)), expr(std::move(e)) {}

    IfNode::IfNode(
        std::unique_ptr<ExprNode> c,
        std::unique_ptr<ExprNode> t,
        std::unique_ptr<ExprNode> e)
        : cond(std::move(c)), thenExpr(std::move(t)), elseExpr(std::move(e)) {}

    WhileNode::WhileNode(
        std::unique_ptr<ExprNode> c,
        std::unique_ptr<ExprNode> b)
        : cond(std::move(c)), body(std::move(b)) {}

    ForNode::ForNode(
        const std::string &v,
        std::unique_ptr<ExprNode> it,
        std::unique_ptr<ExprNode> b)
        : var(v), iterable(std::move(it)), body(std::move(b)) {}

    AssignNode::AssignNode(
        const std::string &v,
        std::unique_ptr<ExprNode> e)
        : var(v), expr(std::move(e)) {}

    BinaryOpNode::BinaryOpNode(
        const std::string &o,
        std::unique_ptr<ExprNode> l,
        std::unique_ptr<ExprNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    UnaryOpNode::UnaryOpNode(
        const std::string &o,
        std::unique_ptr<ExprNode> opd)
        : op(o), operand(std::move(opd)) {}

    FuncCallNode::FuncCallNode(
        const std::string &n,
        std::vector<std::unique_ptr<ExprNode>> a)
        : name(n), args(std::move(a)) {}

    MethodCallNode::MethodCallNode(
        std::unique_ptr<ExprNode> r,
        const std::string &n,
        std::vector<std::unique_ptr<ExprNode>> a)
        : receiver(std::move(r)), name(n), args(std::move(a)) {}

    NewNode::NewNode(
        const std::string &tn,
        std::vector<std::unique_ptr<ExprNode>> a)
        : typeName(tn), args(std::move(a)) {}

    BlockNode::BlockNode(
        std::vector<std::unique_ptr<ExprNode>> stmts)
        : statements(std::move(stmts)) {}

} // namespace hulk