#include <iostream>
#include <fstream>
#include <exception>
#include "lexer/HULKLexer.h"
#include "parser/HULKParser.h"

int main(int argc, char **argv)
{
    try
    {
        std::string code;
        if (argc > 1)
        {
            std::ifstream in(argv[1]);
            code.assign(std::istreambuf_iterator<char>(in),
                        std::istreambuf_iterator<char>());
        }
        else
        {
            std::getline(std::cin, code);
        }

        std::cerr << "[Main] Input code: " << code << "\n";

        HULKLexer lexer(code);
        HULKParser parser(lexer);

        std::cerr << "[Main] Parsing...\n";
        ASTPtr tree = parser.parse();

        std::cerr << "[Main] Evaluating AST...\n";
        double result = tree->evaluate();

        std::cerr << "[Main] Final result = " << result << "\n";
        std::cout << result << std::endl;

        delete tree;
        return 0;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "[Main] Error: " << ex.what() << std::endl;
        return 1;
    }
}
