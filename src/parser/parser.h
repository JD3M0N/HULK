// parser.h
#ifndef HULK_PARSER_H
#define HULK_PARSER_H

#include <memory>
#include <vector>
#include <stdexcept>
#include "../src/lexer/lexer.h" // Tu implementación de Lexer y Token
#include "../src/AST/ast.h"     // Definiciones de nodos AST: ProgramNode, ExprNode, etc.

class Parser
{
public:
    explicit Parser(Lexer &lexer);
    std::unique_ptr<ProgramNode> parseProgram();

private:
    Lexer &lexer;
    Token cur; // token actual

    void advance();         // avanzar al siguiente token
    bool accept(TokenType); // si cur.type == esperado, avanza y devuelve true
    void expect(TokenType); // si cur.type != esperado lanza error, si no avanza

    // 2. Definitions
    std::vector<std::unique_ptr<DefinitionNode>> parseDefinitionList();
    std::unique_ptr<DefinitionNode> parseDefinition();
    std::unique_ptr<FuncDefNode> parseFuncDef();
    std::unique_ptr<TypeDefNode> parseTypeDef();
    std::unique_ptr<ProtoDefNode> parseProtoDef();

    // 3. Global expression
    std::unique_ptr<ExprNode> parseGlobalExpr();

    // 4. Expressions (SimpleExpr)
    std::unique_ptr<ExprNode> parseExpr();
    std::unique_ptr<ExprNode> parseLetInStmt();
    std::unique_ptr<ExprNode> parseIfExpr();
    std::unique_ptr<ExprNode> parseWhileExpr();
    std::unique_ptr<ExprNode> parseForExpr();
    std::unique_ptr<ExprNode> parseDestructiveAssign();
    std::unique_ptr<ExprNode> parseOrExpr();

    // 5. Precedencia de operadores
    std::unique_ptr<ExprNode> parseAndExpr();
    std::unique_ptr<ExprNode> parseEqExpr();
    std::unique_ptr<ExprNode> parseRelExpr();
    std::unique_ptr<ExprNode> parseConcatExpr();
    std::unique_ptr<ExprNode> parseAddExpr();
    std::unique_ptr<ExprNode> parseMulExpr();
    std::unique_ptr<ExprNode> parseUnaryExpr();
    std::unique_ptr<ExprNode> parsePowExpr();

    // 6. Primarios y literales
    std::unique_ptr<ExprNode> parsePrimary();
    std::unique_ptr<ExprNode> parseLiteral();
    std::unique_ptr<ExprNode> parseFuncCallOrVar();
    std::unique_ptr<ExprNode> parseMethodCallChain(std::unique_ptr<ExprNode> receiver);
    std::unique_ptr<ExprNode> parseNewType();
    std::unique_ptr<ExprNode> parseExprBlock();

    // 7. Listas y parámetros
    std::vector<std::unique_ptr<ExprNode>> parseExprList();
    std::vector<std::unique_ptr<ExprNode>> parseArgList();
    std::vector<std::unique_ptr<ParamNode>> parseParamList();
    std::vector<std::unique_ptr<ParamNode>> parseTypedParamList();

    // 8. Miembros de tipos y protocolos
    std::vector<std::unique_ptr<MemberNode>> parseMemberList();
    std::unique_ptr<MemberNode> parseAttrDef();
    std::unique_ptr<MemberNode> parseMethodDef();
    std::unique_ptr<ProtoMemberNode> parseProtoMember();

    // Auxiliares
    std::unique_ptr<TypeNode> parseType();
};

#endif // HULK_PARSER_H
