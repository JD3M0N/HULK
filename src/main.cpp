#include <cstdio>
#include <iostream>

#include "AST/ast.hpp"
#include "Evaluator/evaluator.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Value/value.hpp"
#include "Scope/scope.hpp"
#include "Scope/name_resolver.hpp"

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

    if (yyparse() != 0 || rootAST == nullptr)
    {
        std::cerr << "Error al parsear.\n";
        fclose(file);
        return 1;
    }

    // 1) Análisis semántico: resolución de nombres usando Scope
    try
    {
        NameResolver resolver;
        rootAST->accept(&resolver);
        std::cout << "=== Análisis semántico OK ===\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error semántico: " << e.what() << "\n";
        fclose(file);
        return 2;
    }

    // 2) Pretty-print del AST completo
    std::cout << "=== AST ===\n";
    PrintVisitor printer;
    rootAST->accept(&printer);

    // 3) Ejecución
    std::cout << "\n=== Ejecución ===\n";
    EvaluatorVisitor evaluator;
    rootAST->accept(&evaluator);

    fclose(file);
    return 0;
}
