#include <cstdio>
#include <iostream>
#include <fstream>

#include "AST/ast.hpp"
#include "Scope/name_resolver.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Value/value.hpp"
#include "CodeGen/cil_generator.hpp"
#include "CodeGen/mips_generator.hpp"  

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

    std::string input_file = argv[1];
    

    FILE *file = std::fopen(input_file.c_str(), "r");
    if (!file)
    {
        std::cerr << "No se pudo abrir el archivo: " << input_file << "\n";
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

    // 6) Generación de MIPS (opcional)
    
    std::cout << "\n=== Generacion de Codigo MIPS ===\n";
    try {
        MIPSGenerator mipsGen;
        std::string mipsCode = mipsGen.generateMIPS(cilCode);
            
        // Guardar en archivo .s
        std::string output_file = input_file.substr(0, input_file.find_last_of('.')) + ".s";
        std::ofstream mips_file(output_file);
        mips_file << mipsCode;
        mips_file.close();
            
        std::cout << "Código MIPS generado en: " << output_file << std::endl;
        std::cout << "\n" << mipsCode << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Error en generación MIPS: " << e.what() << "\n";
        std::fclose(file);
        return 7;
    }
    
    // // 7) Ejecución del código CIL (comportamiento original)
    // std::cout << "\n=== Ejecucion CIL ===\n";
    // try
    // {
    //     CILInterpreter interpreter;
    //     interpreter.loadProgram(cilCode);
    //     interpreter.execute();
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error en ejecución CIL: " << e.what() << "\n";
    //     std::fclose(file);
    //     return 6;
    // }
    

    fclose(file);
    return 0;
}
