// parser.cpp
#include "parser.h"

// Constructor: inicializa y carga el primer token
Parser::Parser(Lexer &lex) : lexer(lex)
{
    advance();
}

void Parser::advance()
{
    cur = lexer.nextToken();
}

bool Parser::accept(TokenType tt)
{
    if (cur.type == tt)
    {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType tt)
{
    if (cur.type != tt)
        throw std::runtime_error("Parse error: se esperaba " + tokenName(tt));
    advance();
}

// parseProgram ::= DefinitionList GlobalExpr
std::unique_ptr<ProgramNode> Parser::parseProgram()
{
    auto defs = parseDefinitionList();
    auto expr = parseGlobalExpr();
    return std::make_unique<ProgramNode>(std::move(defs), std::move(expr));
}

// DefinitionList ::= Definition DefinitionList | ε
std::vector<std::unique_ptr<DefinitionNode>> Parser::parseDefinitionList()
{
    std::vector<std::unique_ptr<DefinitionNode>> defs;
    while (cur.type == TokenType::KW_function || cur.type == TokenType::KW_type || cur.type == TokenType::KW_protocol)
    {
        defs.push_back(parseDefinition());
    }
    return defs;
}

std::unique_ptr<DefinitionNode> Parser::parseDefinition()
{
    if (cur.type == TokenType::KW_function)
        return parseFuncDef();
    if (cur.type == TokenType::KW_type)
        return parseTypeDef();
    // protocol
    return parseProtoDef();
}

// FuncDef ::= "function" id "(" ParamList? ")" ReturnType? "=>" SimpleExpr ";"
//           | "function" id "(" ParamList? ")" ReturnType? "{" ExprList "}" ";"
std::unique_ptr<FuncDefNode> Parser::parseFuncDef()
{
    expect(TokenType::KW_function);
    std::string name = cur.text;
    expect(TokenType::IDENT);
    expect(TokenType::LPAREN);
    auto params = (cur.type != TokenType::RPAREN) ? parseParamList() : std::vector<std::unique_ptr<ParamNode>>{};
    expect(TokenType::RPAREN);
    std::unique_ptr<TypeNode> retType = nullptr;
    if (accept(TokenType::COLON))
        retType = parseType();

    std::unique_ptr<ExprNode> body;
    if (accept(TokenType::ARROW))
    {
        body = parseExpr();
        expect(TokenType::SEMICOLON);
    }
    else
    {
        expect(TokenType::LBRACE);
        auto stmts = parseExprList();
        expect(TokenType::RBRACE);
        expect(TokenType::SEMICOLON);
        body = std::make_unique<BlockNode>(std::move(stmts));
    }
    return std::make_unique<FuncDefNode>(name, std::move(params), std::move(retType), std::move(body));
}

// TypeDef ::= "type" id TypeParams? ("inherits" id "(" ArgList? ")")? "{" MemberList "}"
std::unique_ptr<TypeDefNode> Parser::parseTypeDef()
{
    expect(TokenType::KW_type);
    std::string name = cur.text;
    expect(TokenType::IDENT);
    // (opcional) params genéricos...
    if (accept(TokenType::LPAREN))
    {
        // parseTypeParams();
        while (!accept(TokenType::RPAREN))
            advance();
    }
    // herencia opcional...
    if (accept(TokenType::KW_inherits))
    {
        std::string base = cur.text;
        expect(TokenType::IDENT);
        expect(TokenType::LPAREN);
        // ArgList opcional
        if (cur.type != TokenType::RPAREN)
            parseArgList();
        expect(TokenType::RPAREN);
    }
    expect(TokenType::LBRACE);
    auto members = parseMemberList();
    expect(TokenType::RBRACE);
    return std::make_unique<TypeDefNode>(name, std::move(members));
}

// ProtoDef ::= "protocol" id ("extends" id)? "{" ProtoMember* "}"
std::unique_ptr<ProtoDefNode> Parser::parseProtoDef()
{
    expect(TokenType::KW_protocol);
    std::string name = cur.text;
    expect(TokenType::IDENT);
    if (accept(TokenType::KW_extends))
    {
        std::string parent = cur.text;
        expect(TokenType::IDENT);
    }
    expect(TokenType::LBRACE);
    std::vector<std::unique_ptr<ProtoMemberNode>> members;
    while (cur.type == TokenType::IDENT)
    {
        members.push_back(parseProtoMember());
    }
    expect(TokenType::RBRACE);
    return std::make_unique<ProtoDefNode>(name, std::move(members));
}

// GlobalExpr ::= Expr [";"]
std::unique_ptr<ExprNode> Parser::parseGlobalExpr()
{
    auto e = parseExpr();
    accept(TokenType::SEMICOLON);
    return e;
}

// parseExpr y resto de funciones siguen la misma mecánica:
//   - comprobar qué token actual nos lleva a cuál producción
//   - invocar al parseXxx() correspondiente
//   - combinar nodos AST con std::make_unique<...>(...)

std::unique_ptr<ExprNode> Parser::parseExpr()
{
    // empieza en LetInStmt | IfExpr | WhileExpr | ForExpr | DestructiveAssign | OrExpr
    if (cur.type == TokenType::KW_let)
        return parseLetInStmt();
    if (cur.type == TokenType::KW_if)
        return parseIfExpr();
    if (cur.type == TokenType::KW_while)
        return parseWhileExpr();
    if (cur.type == TokenType::KW_for)
        return parseForExpr();
    if (cur.type == TokenType::IDENT && lexer.peek().type == TokenType::COLON_EQ)
        return parseDestructiveAssign();
    return parseOrExpr();
}

// … implementación de parseLetInStmt, parseIfExpr, parseWhileExpr, parseForExpr,
// parseDestructiveAssign, y después los métodos de precedencia:
// parseOrExpr → parseAndExpr → parseEqExpr → … → parsePowExpr → parsePrimary

// parsePrimary() maneja literales, identificadores, llamadas, new, bloques, paréntesis…
std::unique_ptr<ExprNode> Parser::parsePrimary()
{
    switch (cur.type)
    {
    case TokenType::NUMBER:
        return parseLiteral();
    case TokenType::STRING:
        return parseLiteral();
    case TokenType::KW_true:
    case TokenType::KW_false:
        return parseLiteral();
    case TokenType::IDENT:
        return parseFuncCallOrVar();
    case TokenType::KW_new:
        return parseNewType();
    case TokenType::LBRACE:
        return parseExprBlock();
    case TokenType::LPAREN:
    {
        advance();
        auto e = parseExpr();
        expect(TokenType::RPAREN);
        return e;
    }
    default:
        throw std::runtime_error("Primary expected");
    }
}

// …etcétera para el resto de nodos…
