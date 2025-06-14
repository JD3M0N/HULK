#include <cstdio>
#include <iostream>

#include "AST/ast.hpp"
#include "Scope/name_resolver.hpp"
#include "Type/type_inferer.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Value/value.hpp"
#include "CodeGen/cil_generator.hpp"
#include "CodeGen/cil_interpreter.hpp"

extern FILE *yyin;
extern int yyparse();
extern int yylineno;
extern Program *rootAST;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <script.hulk>\n";
        return 1;
    }

    FILE *file = std::fopen(argv[1], "r");
    if (!file)
    {
        std::cerr << "No se pudo abrir el archivo: " << argv[1] << "\n";
        return 1;
    }
    yyin = file;

    // 1) Parsing
    if (yyparse() != 0 || rootAST == nullptr)
    {
        std::cerr << "Error al parsear.\n";
        std::fclose(file);
        return 1;
    }

    // 2) Resolución de nombres
    std::cout << "=== Resolucion de nombres ===\n";
    try
    {
        NameResolver nameResolver;
        rootAST->accept(&nameResolver);
        std::cout << "=== Resolucion de nombres OK ===\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error de resolucion de nombres: " << e.what() << "\n";
        std::fclose(file);
        return 2;
    }

    // 3) Inferencia de tipos
    std::cout << "=== Inferencia de tipos ===\n";
    try
    {
        TypeInfererVisitor typeInferer;
        rootAST->accept(&typeInferer);
        std::cout << "=== Inferencia de tipos OK ===\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error de inferencia de tipos: " << e.what() << "\n";
        std::fclose(file);
        return 3;
    }

    // 4) Pretty-print del AST
    std::cout << "=== AST ===\n";
    PrintVisitor printer;
    rootAST->accept(&printer);

    // 5) Generación de código CIL
    std::cout << "\n=== Generacion de Codigo CIL ===\n";
    std::string cilCode;
    try
    {
        CILGenerator cilGen;
        cilCode = cilGen.generateCode(rootAST);
        std::cout << cilCode << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error en generación de código: " << e.what() << "\n";
        std::fclose(file);
        return 5;
    }

    // 6) Ejecución del código CIL
    std::cout << "\n=== Ejecucion CIL ===\n";
    try
    {
        CILInterpreter interpreter;
        interpreter.loadProgram(cilCode);
        interpreter.execute();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error en ejecución CIL: " << e.what() << "\n";
        std::fclose(file);
        return 6;
    }

    fclose(file);
    return 0;
}
