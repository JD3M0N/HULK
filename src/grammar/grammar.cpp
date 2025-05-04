// grammar.cpp
#include "grammar.h"
#include <algorithm>

Grammar::Grammar()
{
    initGrammar();
}

const std::vector<Production> &
Grammar::getProductions(const std::string &nt) const
{
    static const std::vector<Production> empty;
    auto it = productions.find(nt);
    return it != productions.end() ? it->second : empty;
}

const std::string &Grammar::getStartSymbol() const
{
    return startSymbol;
}

bool Grammar::isNonTerminal(const std::string &s) const
{
    return std::find(nonTerminals.begin(), nonTerminals.end(), s) != nonTerminals.end();
}

bool Grammar::isTerminal(const std::string &s) const
{
    return std::find(terminals.begin(), terminals.end(), s) != terminals.end();
}

const std::vector<std::string> &Grammar::getNonTerminals() const
{
    return nonTerminals;
}

const std::vector<std::string> &Grammar::getTerminals() const
{
    return terminals;
}

void Grammar::initGrammar()
{
    // 1. Símbolo inicial
    startSymbol = "Program";

    // 2. No terminales
    nonTerminals = {
        "Program",
        "DefinitionList", "Definition",
        "FuncDef", "TypeDef", "ProtoDef",
        "GlobalExpr",
        "Expr", "LetInStmt", "IfExpr", "WhileExpr", "ForExpr", "DestructiveAssign",
        "OrExpr", "AndExpr", "EqExpr", "RelExpr", "ConcatExpr",
        "AddExpr", "MulExpr", "UnaryExpr", "PowExpr",
        "Primary", "Literal", "FuncCallOrVar", "NewType", "ExprBlock",
        "ExprList", "ArgList",
        "ParamList", "TypedParamList",
        "MemberList", "AttrDef", "MethodDef", "ProtoMember",
        "Type"};

    // 3. Terminales (tokens de tu Lexer)
    terminals = {
        "KW_function", "KW_type", "KW_protocol", "KW_let", "KW_in", "KW_if",
        "KW_elif", "KW_else", "KW_while", "KW_for", "KW_new",
        "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACK", "RBRACK",
        "COMMA", "SEMICOLON", "ARROW", "COLON_EQ", "COLON",
        "PIPE", "AMP", "EQ", "NEQ", "LT", "LE", "GT", "GE",
        "AT", "ATAT", "+", "-", "*", "/", "%", "POW", "BANG",
        "IDENT", "NUMBER", "STRING", "BOOL"};

    // 4. Producciones

    // Program ::= DefinitionList GlobalExpr
    productions["Program"].push_back({"Program", {"DefinitionList", "GlobalExpr"}});

    // DefinitionList ::= Definition DefinitionList | ε
    productions["DefinitionList"].push_back({"DefinitionList", {"Definition", "DefinitionList"}});
    productions["DefinitionList"].push_back({"DefinitionList", {}});

    // Definition ::= FuncDef | TypeDef | ProtoDef
    productions["Definition"].push_back({"Definition", {"FuncDef"}});
    productions["Definition"].push_back({"Definition", {"TypeDef"}});
    productions["Definition"].push_back({"Definition", {"ProtoDef"}});

    // FuncDef ::= "function" IDENT "(" ParamList? ")" ":"? Type? ARROW Expr SEMICOLON
    //           | "function" IDENT "(" ParamList? ")" ":"? Type? "{" ExprList "}" SEMICOLON
    productions["FuncDef"].push_back(
        {"FuncDef", {"KW_function", "IDENT", "LPAREN", "ParamList", "RPAREN", "COLON", "Type", "ARROW", "Expr", "SEMICOLON"}});
    productions["FuncDef"].push_back(
        {"FuncDef", {"KW_function", "IDENT", "LPAREN", "ParamList", "RPAREN", "COLON", "Type", "LBRACE", "ExprList", "RBRACE", "SEMICOLON"}});

    // TypeDef ::= "type" IDENT "(" ParamList? ")"? "inherits"? IDENT "(" ArgList? ")"? "{" MemberList "}"
    productions["TypeDef"].push_back(
        {"TypeDef", {"KW_type", "IDENT", "LPAREN", "ParamList", "RPAREN", "KW_inherits", "IDENT", "LPAREN", "ArgList", "RPAREN", "LBRACE", "MemberList", "RBRACE"}});

    // ProtoDef ::= "protocol" IDENT ("extends" IDENT)? "{" ProtoMember* "}"
    productions["ProtoDef"].push_back(
        {"ProtoDef", {"KW_protocol", "IDENT", "LBRACE", "ProtoMember", "RBRACE"}});
    productions["ProtoDef"].push_back(
        {"ProtoDef", {"KW_protocol", "IDENT", "KW_extends", "IDENT", "LBRACE", "ProtoMember", "RBRACE"}});

    // GlobalExpr ::= Expr SEMICOLON?
    productions["GlobalExpr"].push_back({"GlobalExpr", {"Expr", "SEMICOLON"}});

    // Expr ::= LetInStmt | IfExpr | WhileExpr | ForExpr | DestructiveAssign | OrExpr
    productions["Expr"].push_back({"Expr", {"LetInStmt"}});
    productions["Expr"].push_back({"Expr", {"IfExpr"}});
    productions["Expr"].push_back({"Expr", {"WhileExpr"}});
    productions["Expr"].push_back({"Expr", {"ForExpr"}});
    productions["Expr"].push_back({"Expr", {"DestructiveAssign"}});
    productions["Expr"].push_back({"Expr", {"OrExpr"}});

    // LetInStmt ::= "let" IDENT "=" Expr (";" | "in" Expr)
    productions["LetInStmt"].push_back(
        {"LetInStmt", {"KW_let", "IDENT", "COLON_EQ", "Expr"}});

    // IfExpr ::= "if" "(" Expr ")" Expr ("elif" "(" Expr ")" Expr)* "else" Expr
    productions["IfExpr"].push_back(
        {"IfExpr", {"KW_if", "LPAREN", "Expr", "RPAREN", "Expr", "KW_else", "Expr"}});
    productions["IfExpr"].push_back(
        {"IfExpr", {"KW_if", "LPAREN", "Expr", "RPAREN", "Expr", "KW_elif", "LPAREN", "Expr", "RPAREN", "Expr", "KW_else", "Expr"}});

    // WhileExpr ::= "while" "(" Expr ")" Expr
    productions["WhileExpr"].push_back(
        {"WhileExpr", {"KW_while", "LPAREN", "Expr", "RPAREN", "Expr"}});

    // ForExpr ::= "for" "(" IDENT "in" Expr ")" Expr
    productions["ForExpr"].push_back(
        {"ForExpr", {"KW_for", "LPAREN", "IDENT", "KW_in", "Expr", "RPAREN", "Expr"}});

    // DestructiveAssign ::= IDENT COLON_EQ Expr
    productions["DestructiveAssign"].push_back(
        {"DestructiveAssign", {"IDENT", "COLON_EQ", "Expr"}});

    // Cadena de precedencia de operadores
    productions["OrExpr"].push_back({"OrExpr", {"AndExpr"}});
    productions["OrExpr"].push_back({"OrExpr", {"OrExpr", "PIPE", "AndExpr"}});
    productions["AndExpr"].push_back({"AndExpr", {"EqExpr"}});
    productions["AndExpr"].push_back({"AndExpr", {"AndExpr", "AMP", "EqExpr"}});
    productions["EqExpr"].push_back({"EqExpr", {"RelExpr"}});
    productions["EqExpr"].push_back({"EqExpr", {"EqExpr", "EQ", "RelExpr"}});
    productions["EqExpr"].push_back({"EqExpr", {"EqExpr", "NEQ", "RelExpr"}});
    productions["RelExpr"].push_back({"RelExpr", {"ConcatExpr"}});
    productions["RelExpr"].push_back({"RelExpr", {"RelExpr", "LT", "ConcatExpr"}});
    productions["RelExpr"].push_back({"RelExpr", {"RelExpr", "LE", "ConcatExpr"}});
    productions["ConcatExpr"].push_back({"ConcatExpr", {"AddExpr"}});
    productions["ConcatExpr"].push_back({"ConcatExpr", {"ConcatExpr", "ATAT", "AddExpr"}});
    productions["AddExpr"].push_back({"AddExpr", {"MulExpr"}});
    productions["AddExpr"].push_back({"AddExpr", {"AddExpr", "+", "MulExpr"}});
    productions["AddExpr"].push_back({"AddExpr", {"AddExpr", "-", "MulExpr"}});
    productions["MulExpr"].push_back({"MulExpr", {"UnaryExpr"}});
    productions["MulExpr"].push_back({"MulExpr", {"MulExpr", "*", "UnaryExpr"}});
    productions["MulExpr"].push_back({"MulExpr", {"MulExpr", "/", "UnaryExpr"}});
    productions["UnaryExpr"].push_back({"UnaryExpr", {"BANG", "UnaryExpr"}});
    productions["UnaryExpr"].push_back({"UnaryExpr", {"AddExpr"}});
    productions["PowExpr"].push_back({"PowExpr", {"Primary"}});
    productions["PowExpr"].push_back({"PowExpr", {"Primary", "POW", "PowExpr"}});

    // Primary ::= Literal | FuncCallOrVar | NewType | ExprBlock | "(" Expr ")"
    productions["Primary"].push_back({"Primary", {"Literal"}});
    productions["Primary"].push_back({"Primary", {"FuncCallOrVar"}});
    productions["Primary"].push_back({"Primary", {"NewType"}});
    productions["Primary"].push_back({"Primary", {"ExprBlock"}});
    productions["Primary"].push_back({"Primary", {"LPAREN", "Expr", "RPAREN"}});

    // Literal ::= NUMBER | STRING | BOOL
    productions["Literal"].push_back({"Literal", {"NUMBER"}});
    productions["Literal"].push_back({"Literal", {"STRING"}});
    productions["Literal"].push_back({"Literal", {"BOOL"}});

    // FuncCallOrVar ::= IDENT "(" ArgList? ")" | IDENT
    productions["FuncCallOrVar"].push_back({"FuncCallOrVar", {"IDENT", "LPAREN", "ArgList", "RPAREN"}});
    productions["FuncCallOrVar"].push_back({"FuncCallOrVar", {"IDENT"}});

    // NewType ::= "new" IDENT "(" ArgList? ")"
    productions["NewType"].push_back({"NewType", {"KW_new", "IDENT", "LPAREN", "ArgList", "RPAREN"}});

    // ExprBlock ::= "{" ExprList "}"
    productions["ExprBlock"].push_back({"ExprBlock", {"LBRACE", "ExprList", "RBRACE"}});

    // ExprList ::= Expr ( SEMICOLON Expr )* SEMICOLON?
    productions["ExprList"].push_back({"ExprList", {"Expr"}});
    productions["ExprList"].push_back({"ExprList", {"ExprList", "SEMICOLON", "Expr"}});

    // ArgList ::= Expr ("," Expr)*
    productions["ArgList"].push_back({"ArgList", {"Expr"}});
    productions["ArgList"].push_back({"ArgList", {"ArgList", "COMMA", "Expr"}});

    // ParamList ::= TypedParam ("," TypedParam)*
    productions["ParamList"].push_back({"ParamList", {"TypedParam"}});
    productions["ParamList"].push_back({"ParamList", {"ParamList", "COMMA", "TypedParam"}});

    // TypedParam ::= IDENT (":" Type)?
    productions["TypedParamList"].push_back({"TypedParamList", {"IDENT", "COLON", "Type"}});
    productions["TypedParamList"].push_back({"TypedParamList", {"IDENT"}});

    // MemberList ::= (AttrDef | MethodDef)*
    productions["MemberList"].push_back({"MemberList", {}}); // vacío
    productions["MemberList"].push_back({"MemberList", {"MemberList", "AttrDef"}});
    productions["MemberList"].push_back({"MemberList", {"MemberList", "MethodDef"}});

    // AttrDef ::= IDENT ":"? Type? COLON_EQ Expr SEMICOLON
    productions["AttrDef"].push_back(
        {"AttrDef", {"IDENT", "COLON", "Type", "COLON_EQ", "Expr", "SEMICOLON"}});

    // MethodDef ::= IDENT "(" ParamList? ")" ":"? Type? ( "=>" Expr SEMICOLON | "{" ExprList "}" SEMICOLON )
    productions["MethodDef"].push_back(
        {"MethodDef", {"IDENT", "LPAREN", "ParamList", "RPAREN", "COLON", "Type", "ARROW", "Expr", "SEMICOLON"}});
    productions["MethodDef"].push_back(
        {"MethodDef", {"IDENT", "LPAREN", "ParamList", "RPAREN", "COLON", "Type", "LBRACE", "ExprList", "RBRACE", "SEMICOLON"}});

    // ProtoMember ::= IDENT "(" TypedParamList? ")" ":" Type SEMICOLON
    productions["ProtoMember"].push_back(
        {"ProtoMember", {"IDENT", "LPAREN", "TypedParamList", "RPAREN", "COLON", "Type", "SEMICOLON"}});

    // Type ::= IDENT
    productions["Type"].push_back({"Type", {"IDENT"}});
}
