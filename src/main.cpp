#include <cstdio>
#include <iostream>
#include <fstream> // Incluir fstream para manejo de archivos

#include "AST/ast.hpp"
#include "Scope/name_resolver.hpp"
#include "Type/type_inferer.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Evaluator/evaluator.hpp"
#include "Value/value.hpp"
#include "Errors/error_handler.hpp"
#include "Errors/error_reporter.hpp"

extern FILE *yyin;
extern int yyparse();
extern int yylineno;
extern Program *rootAST;

int main(int argc, char *argv[])
{
    // Configura el manejador de errores
    ErrorReporter reporter(std::cerr, true); // Usa colores

    // Inicializa el sistema con una función de callback para reportar errores
    ErrorManager::initialize(
        false, // No abortar automáticamente en errores
        ErrorLevel::INFO, // Reportar desde nivel INFO
        [&reporter](const Error &error)
        {
            reporter.report(error);
            // Opcional: Guardar errores en un archivo de log
            static std::ofstream errorLog("error_log.txt", std::ios::app);
            if (errorLog.is_open()) {
                errorLog << error.getFullMessage() << std::endl;
            }
        }
    );

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

    // 5) Ejecución
    std::cout << "\n=== Ejecucion ===\n";
    EvaluatorVisitor evaluator;
    rootAST->accept(&evaluator);

    //  línea a línea:
    // for (auto &stmt : rootAST->stmts)
    //     stmt->accept(&evaluator);

    fclose(file);

    return 0;
}
