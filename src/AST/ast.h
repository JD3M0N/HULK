// ast.h
#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

namespace hulk
{

    // Base AST node
    struct Node
    {
        virtual ~Node() = default;
    };

    using NodePtr = std::unique_ptr<Node>;

    // Forward declarations
    struct ExprNode;
    struct DefinitionNode;
    struct ParamNode;
    struct TypeNode;
    struct MemberNode;

    // Program: definitions + main expression
    struct ProgramNode : Node
    {
        std::vector<std::unique_ptr<DefinitionNode>> definitions;
        std::unique_ptr<ExprNode> mainExpr;
        ProgramNode(std::vector<std::unique_ptr<DefinitionNode>> defs,
                    std::unique_ptr<ExprNode> expr);
    };

    // Definitions
    struct DefinitionNode : Node
    {
        virtual ~DefinitionNode() = default;
    };

    struct FuncDefNode : DefinitionNode
    {
        std::string name;
        std::vector<std::unique_ptr<ParamNode>> params;
        std::unique_ptr<TypeNode> returnType;
        std::unique_ptr<ExprNode> body;
        FuncDefNode(const std::string &n,
                    std::vector<std::unique_ptr<ParamNode>> ps,
                    std::unique_ptr<TypeNode> rt,
                    std::unique_ptr<ExprNode> b);
    };

    struct TypeDefNode : DefinitionNode
    {
        std::string name;
        std::unique_ptr<TypeNode> baseType; // optional
        std::vector<std::unique_ptr<MemberNode>> members;
        TypeDefNode(const std::string &n,
                    std::unique_ptr<TypeNode> base,
                    std::vector<std::unique_ptr<MemberNode>> m);
    };

    struct ProtoDefNode : DefinitionNode
    {
        std::string name;
        std::vector<std::unique_ptr<MemberNode>> methods;
        ProtoDefNode(const std::string &n,
                     std::vector<std::unique_ptr<MemberNode>> ms);
    };

    // Parameters and Types
    struct ParamNode
    {
        std::string name;
        std::unique_ptr<TypeNode> type; // optional
        ParamNode(const std::string &n, std::unique_ptr<TypeNode> t);
    };

    struct TypeNode : Node
    {
        std::string name;
        TypeNode(const std::string &n);
    };

    // Members (attributes or methods)
    struct MemberNode : Node
    {
        virtual ~MemberNode() = default;
    };

    struct AttrDefNode : MemberNode
    {
        std::string name;
        std::unique_ptr<TypeNode> type; // optional
        std::unique_ptr<ExprNode> initExpr;
        AttrDefNode(const std::string &n,
                    std::unique_ptr<TypeNode> t,
                    std::unique_ptr<ExprNode> ie);
    };

    struct MethodDefNode : MemberNode
    {
        std::string name;
        std::vector<std::unique_ptr<ParamNode>> params;
        std::unique_ptr<TypeNode> returnType;
        std::unique_ptr<ExprNode> body;
        MethodDefNode(const std::string &n,
                      std::vector<std::unique_ptr<ParamNode>> ps,
                      std::unique_ptr<TypeNode> rt,
                      std::unique_ptr<ExprNode> b);
    };

    // Expressions
    struct ExprNode : Node
    {
        virtual ~ExprNode() = default;
    };

    struct LiteralNode : ExprNode
    {
        std::string value;
        LiteralNode(const std::string &v);
    };

    struct VarNode : ExprNode
    {
        std::string name;
        VarNode(const std::string &n);
    };

    struct LetInNode : ExprNode
    {
        std::vector<std::unique_ptr<ParamNode>> vars;
        std::unique_ptr<ExprNode> expr;
        LetInNode(std::vector<std::unique_ptr<ParamNode>> v,
                  std::unique_ptr<ExprNode> e);
    };

    struct IfNode : ExprNode
    {
        std::unique_ptr<ExprNode> cond;
        std::unique_ptr<ExprNode> thenExpr;
        std::unique_ptr<ExprNode> elseExpr;
        IfNode(std::unique_ptr<ExprNode> c,
               std::unique_ptr<ExprNode> t,
               std::unique_ptr<ExprNode> e);
    };

    struct WhileNode : ExprNode
    {
        std::unique_ptr<ExprNode> cond;
        std::unique_ptr<ExprNode> body;
        WhileNode(std::unique_ptr<ExprNode> c,
                  std::unique_ptr<ExprNode> b);
    };

    struct ForNode : ExprNode
    {
        std::string var;
        std::unique_ptr<ExprNode> iterable;
        std::unique_ptr<ExprNode> body;
        ForNode(const std::string &v,
                std::unique_ptr<ExprNode> it,
                std::unique_ptr<ExprNode> b);
    };

    struct AssignNode : ExprNode
    {
        std::string var;
        std::unique_ptr<ExprNode> expr;
        AssignNode(const std::string &v,
                   std::unique_ptr<ExprNode> e);
    };

    struct BinaryOpNode : ExprNode
    {
        std::string op;
        std::unique_ptr<ExprNode> left, right;
        BinaryOpNode(const std::string &o,
                     std::unique_ptr<ExprNode> l,
                     std::unique_ptr<ExprNode> r);
    };

    struct UnaryOpNode : ExprNode
    {
        std::string op;
        std::unique_ptr<ExprNode> operand;
        UnaryOpNode(const std::string &o,
                    std::unique_ptr<ExprNode> opd);
    };

    struct FuncCallNode : ExprNode
    {
        std::string name;
        std::vector<std::unique_ptr<ExprNode>> args;
        FuncCallNode(const std::string &n,
                     std::vector<std::unique_ptr<ExprNode>> a);
    };

    struct MethodCallNode : ExprNode
    {
        std::unique_ptr<ExprNode> receiver;
        std::string name;
        std::vector<std::unique_ptr<ExprNode>> args;
        MethodCallNode(std::unique_ptr<ExprNode> r,
                       const std::string &n,
                       std::vector<std::unique_ptr<ExprNode>> a);
    };

    struct NewNode : ExprNode
    {
        std::string typeName;
        std::vector<std::unique_ptr<ExprNode>> args;
        NewNode(const std::string &tn,
                std::vector<std::unique_ptr<ExprNode>> a);
    };

    struct BlockNode : ExprNode
    {
        std::vector<std::unique_ptr<ExprNode>> statements;
        BlockNode(std::vector<std::unique_ptr<ExprNode>> stmts);
    };

} // namespace hulk

#endif // AST_H
