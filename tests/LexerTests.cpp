
#include <iostream>
#include <string>
#include <vector>
#include "../src/lexer/Lexer.h"

using namespace std;

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::Number:
        return "Number";
    case TokenType::String:
        return "String";
    case TokenType::Boolean:
        return "Boolean";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::Keyword:
        return "Keyword";
    case TokenType::Plus:
        return "Plus";
    case TokenType::Minus:
        return "Minus";
    case TokenType::Multiply:
        return "Multiply";
    case TokenType::Divide:
        return "Divide";
    case TokenType::Equal:
        return "Equal";
    case TokenType::Equal_Equal:
        return "Equal_Equal";
    case TokenType::Not_Equal:
        return "Not_Equal";
    case TokenType::Less:
        return "Less";
    case TokenType::Less_Equal:
        return "Less_Equal";
    case TokenType::Greater:
        return "Greater";
    case TokenType::Greater_Equal:
        return "Greater_Equal";
    case TokenType::And:
        return "And";
    case TokenType::Or:
        return "Or";
    case TokenType::Left_Paren:
        return "Left_Paren";
    case TokenType::Right_Paren:
        return "Right_Paren";
    case TokenType::Left_Brace:
        return "Left_Brace";
    case TokenType::Right_Brace:
        return "Right_Brace";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::Comma:
        return "Comma";
    case TokenType::EOF_TOKEN:
        return "EOF";
    default:
        return "Unknown";
    }
}

int main()
{
    cout << "Ingrese el texto a analizar por el Lexer:" << endl;

    // Nota- adaptar para leer varias líneas luego
    string input;
    getline(cin, input);

    // Crear una instancia del Lexer con el texto ingresado
    Lexer lexer(input);

    vector<Token> tokens = lexer.tokenize();

    cout << "\nResultados del anaisis lexico:" << endl;
    for (const auto &token : tokens)
    {
        cout << "Token: '" << token.lexeme << "'\t"
             << "Tipo: " << tokenTypeToString(token.type) << "\t"
             << "Linea: " << token.line << "\t"
             << "Columna: " << token.column << endl;
    }

    return 0;
}
