#include <cstdio>
#include <iostream>
#include <fstream>

#include "AST/ast.hpp"
#include "Scope/name_resolver.hpp"
#include "Type/type_inferer.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Value/value.hpp"
#include "CodeGen/cil_generator.hpp"
#include "CodeGen/mips_generator.hpp"
#include "Error/error_handler.hpp"
#include "Error/compilation_exception.hpp"

extern FILE *yyin;
extern int yyparse();
extern int yylineno;
extern Program *rootAST;

int main(int argc, char *argv[])
{
    // Inicializar el manejador de errores
    ErrorHandler &errorHandler = ErrorHandler::getInstance();
    errorHandler.setMaxErrors(20); // Permitir hasta 20 errores antes de abortar

    if (argc < 2)
    {
        std::cerr << "Uso: " << argv[0] << " <script.hulk>\n";
        return 1;
    }

    std::string input_file = argv[1];
    SourceLocation mainLocation(input_file, 0, 0);

    try
    {
        FILE *file = std::fopen(input_file.c_str(), "r");
        if (!file)
        {
            errorHandler.reportFatal(ErrorType::INTERNAL_ERROR,
                                     "No se pudo abrir el archivo: " + input_file,
                                     mainLocation);
            return 1;
        }
        yyin = file;

        std::cout << "=== Iniciando compilación de " << input_file << " ===\n";

        // 1) Parsing
        std::cout << "=== Fase de Análisis Sintáctico ===\n";
        try
        {
            if (yyparse() != 0 || rootAST == nullptr)
            {
                errorHandler.reportError(ErrorType::SYNTAX_ERROR,
                                         "Error durante el análisis sintáctico",
                                         SourceLocation(input_file, yylineno, 0),
                                         "El parser no pudo procesar el archivo");

                std::fclose(file);
                errorHandler.printErrors();
                errorHandler.printSummary();
                return 2;
            }
            std::cout << "✓ Análisis sintáctico completado exitosamente\n";
        }
        catch (const CompilationException &e)
        {
            errorHandler.reportError(e.getErrorType(), e.what(), e.getLocation());
            std::fclose(file);
            errorHandler.printErrors();
            errorHandler.printSummary();
            return 2;
        }

        // 2) Resolución de nombres
        std::cout << "=== Fase de Resolución de Nombres ===\n";
        try
        {
            NameResolver nameResolver;
            rootAST->accept(&nameResolver);
            std::cout << "✓ Resolución de nombres completada exitosamente\n";
        }
        catch (const CompilationException &e)
        {
            errorHandler.reportError(e.getErrorType(), e.what(), e.getLocation());
        }
        catch (const std::exception &e)
        {
            errorHandler.reportError(ErrorType::SEMANTIC_ERROR,
                                     "Error de resolución de nombres: " + std::string(e.what()),
                                     mainLocation);
        }

        // Verificar si debemos abortar después de la resolución de nombres
        if (errorHandler.shouldAbortCompilation())
        {
            std::fclose(file);
            errorHandler.printErrors();
            errorHandler.printSummary();
            return 3;
        }

        // 3) Inferencia de tipos
        std::cout << "=== Fase de Inferencia de Tipos ===\n";
        try
        {
            TypeInfererVisitor typeInferer;
            rootAST->accept(&typeInferer);
            std::cout << "✓ Inferencia de tipos completada exitosamente\n";
        }
        catch (const CompilationException &e)
        {
            errorHandler.reportError(e.getErrorType(), e.what(), e.getLocation());
        }
        catch (const std::exception &e)
        {
            errorHandler.reportError(ErrorType::TYPE_ERROR,
                                     "Error de inferencia de tipos: " + std::string(e.what()),
                                     mainLocation);
        }

        // Verificar si debemos abortar después de la inferencia de tipos
        if (errorHandler.shouldAbortCompilation())
        {
            std::fclose(file);
            errorHandler.printErrors();
            errorHandler.printSummary();
            return 4;
        }

        // 4) Pretty-print del AST (solo si no hay errores)
        if (!errorHandler.hasErrors())
        {
            std::cout << "=== AST ===\n";
            try
            {
                PrintVisitor printer;
                rootAST->accept(&printer);
            }
            catch (const std::exception &e)
            {
                errorHandler.reportWarning(ErrorType::INTERNAL_ERROR,
                                           "Error al imprimir AST: " + std::string(e.what()),
                                           mainLocation);
            }
        }

        // 5) Generación de código CIL
        std::cout << "\n=== Fase de Generación de Código CIL ===\n";
        std::string cilCode;
        try
        {
            CILGenerator cilGen;
            cilCode = cilGen.generateCode(rootAST);
            std::cout << "✓ Generación de código CIL completada exitosamente\n";
            std::cout << cilCode << std::endl;
        }
        catch (const CompilationException &e)
        {
            errorHandler.reportError(e.getErrorType(), e.what(), e.getLocation());
        }
        catch (const std::exception &e)
        {
            errorHandler.reportError(ErrorType::CODEGEN_ERROR,
                                     "Error en generación de código CIL: " + std::string(e.what()),
                                     mainLocation);
        }

        // Verificar si debemos abortar después de la generación CIL
        if (errorHandler.shouldAbortCompilation())
        {
            std::fclose(file);
            errorHandler.printErrors();
            errorHandler.printSummary();
            return 5;
        }

        // 6) Generación de MIPS (solo si CIL fue exitoso)
        if (!cilCode.empty())
        {
            std::cout << "\n=== Fase de Generación de Código MIPS ===\n";
            try
            {
                MIPSGenerator mipsGen;
                std::string mipsCode = mipsGen.generateMIPS(cilCode);

                // Guardar en archivo .s
                std::string output_file = input_file.substr(0, input_file.find_last_of('.')) + ".s";
                std::ofstream mips_file(output_file);
                if (mips_file.is_open())
                {
                    mips_file << mipsCode;
                    mips_file.close();
                    std::cout << "✓ Código MIPS generado en: " << output_file << std::endl;
                    std::cout << "\n"
                              << mipsCode << std::endl;
                }
                else
                {
                    errorHandler.reportError(ErrorType::INTERNAL_ERROR,
                                             "No se pudo escribir el archivo de salida: " + output_file,
                                             mainLocation);
                }
            }
            catch (const CompilationException &e)
            {
                errorHandler.reportError(e.getErrorType(), e.what(), e.getLocation());
            }
            catch (const std::exception &e)
            {
                errorHandler.reportError(ErrorType::CODEGEN_ERROR,
                                         "Error en generación MIPS: " + std::string(e.what()),
                                         mainLocation);
            }
        }

        fclose(file);

        // Imprimir reporte final
        if (errorHandler.hasErrors() || errorHandler.hasWarnings())
        {
            errorHandler.printErrors();
        }
        errorHandler.printSummary();

        return errorHandler.hasErrors() ? 6 : 0;
    }
    catch (const CompilationException &e)
    {
        errorHandler.reportFatal(e.getErrorType(), e.what(), e.getLocation());
        errorHandler.printErrors();
        errorHandler.printSummary();
        return 7;
    }
    catch (const std::exception &e)
    {
        errorHandler.reportFatal(ErrorType::INTERNAL_ERROR,
                                 "Error inesperado: " + std::string(e.what()),
                                 mainLocation);
        errorHandler.printErrors();
        errorHandler.printSummary();
        return 8;
    }
}
