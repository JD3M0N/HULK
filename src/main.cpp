#include <cstdio>
#include <iostream>

#include "AST/ast.hpp"
#include "Evaluator/evaluator.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "Value/value.hpp"
#include "Errors/error_handler.hpp"
#include "Errors/error_reporter.hpp"
#include "SemanticAnalysis/semantic_validator.hpp"

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
        }
    );

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
        fclose(file);
        ErrorManager::destroy();
        return 1;
    }

    // 1) Pretty-print del AST completo
    std::cout << "=== AST ===\n";
    PrintVisitor printer;
    rootAST->accept(&printer);

    // 2) VALIDACIÓN SEMÁNTICA (NUEVO)
    std::cout << "\n=== Validación Semántica ===\n";
    IntegratedSemanticValidator validator;
    
    bool isSemanticValid = validator.validateProgram(rootAST);
    
    if (!isSemanticValid) {
        std::cout << "Errores semánticos encontrados:\n";
        for (const auto& error : validator.getErrors()) {
            std::cout << "  " << error->getFullMessage() << "\n";
        }
        
        fclose(file);
        ErrorManager::destroy();
        return 1; // Salir si hay errores semánticos
    }
    
    std::cout << "Validación semántica exitosa.\n";

    // 3) Ejecucion (solo si no hay errores semánticos)
    std::cout << "\n=== Ejecución ===\n";
    EvaluatorVisitor evaluator;
    // Puedes ejecutar todo de una vez:
    rootAST->accept(&evaluator);

    //  línea a línea:
    // for (auto &stmt : rootAST->stmts)
    //     stmt->accept(&evaluator);

    fclose(file);

    // Limpia el sistema de errores al finalizar
    ErrorManager::destroy();

    return 0;
}