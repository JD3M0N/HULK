#include <cstdio>
#include <iostream>

#include "AST/ast.hpp"
#include "Evaluator/evaluator.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Value/value.hpp"

extern FILE *yyin;
extern int yyparse();
extern int yylineno;
extern Program *rootAST;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <archivo.hulk>" << std::endl;
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        std::cerr << "Error: No se pudo abrir el archivo: " << argv[1] << std::endl;
        return 1;
    }

    yylineno = 1;
    yyin = file;
    // extern int yydebug;
    // yydebug = 1;

    if (yyparse() != 0 || rootAST == nullptr)
    {
        std::cerr << "Error al parsear.\n";
        return 1;
    }

    // 1) Pretty-print del AST completo
    std::cout << "=== AST ===\n";
    PrintVisitor printer;
    rootAST->accept(&printer);

    // 2)(sentencia a sentencia)
    std::cout << "\n=== Ejecución ===\n";
    EvaluatorVisitor evaluator;
    // Puedes ejecutar todo de una vez:
    rootAST->accept(&evaluator);

    //  línea a línea:
    // for (auto &stmt : rootAST->stmts)
    //     stmt->accept(&evaluator);

    fclose(file);
    return 0;
}