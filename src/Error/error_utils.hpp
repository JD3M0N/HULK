#ifndef ERROR_UTILS_HPP
#define ERROR_UTILS_HPP

#include "error_handler.hpp"
#include <fstream>
#include <sstream>

class ErrorUtils
{
public:
    // Función para obtener el contexto de una línea específica de un archivo
    static std::string getLineContext(const std::string &filename, int lineNumber)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return "";
        }

        std::string line;
        int currentLine = 1;

        while (std::getline(file, line) && currentLine <= lineNumber)
        {
            if (currentLine == lineNumber)
            {
                return line;
            }
            currentLine++;
        }

        return "";
    }

    // Función para obtener múltiples líneas de contexto
    static std::string getMultiLineContext(const std::string &filename, int startLine, int endLine)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return "";
        }

        std::string line;
        std::stringstream context;
        int currentLine = 1;

        while (std::getline(file, line) && currentLine <= endLine)
        {
            if (currentLine >= startLine)
            {
                context << currentLine << ": " << line << "\n";
            }
            currentLine++;
        }

        return context.str();
    }

    // Función para resaltar una posición específica en una línea
    static std::string highlightPosition(const std::string &line, int column)
    {
        std::stringstream result;
        result << line << "\n";

        // Agregar indicador de posición
        for (int i = 0; i < column - 1; ++i)
        {
            result << " ";
        }
        result << "^";

        return result.str();
    }

    // Función para validar que un archivo existe y es legible
    static bool validateFile(const std::string &filename, ErrorHandler &errorHandler)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            errorHandler.reportError(ErrorType::INTERNAL_ERROR,
                                     "No se puede abrir el archivo: " + filename,
                                     SourceLocation(filename, 0, 0));
            return false;
        }
        return true;
    }

    // Función para crear un reporte de errores en formato HTML
    static std::string generateHTMLReport(const ErrorHandler &errorHandler)
    {
        std::stringstream html;

        html << "<!DOCTYPE html>\n";
        html << "<html><head><title>Reporte de Compilación HULK</title>\n";
        html << "<style>\n";
        html << "body { font-family: monospace; margin: 20px; }\n";
        html << ".error { color: red; background-color: #ffe6e6; padding: 5px; margin: 5px 0; }\n";
        html << ".warning { color: orange; background-color: #fff3cd; padding: 5px; margin: 5px 0; }\n";
        html << ".success { color: green; background-color: #d4edda; padding: 5px; margin: 5px 0; }\n";
        html << ".location { font-weight: bold; }\n";
        html << ".context { background-color: #f8f9fa; padding: 10px; margin: 5px 0; }\n";
        html << "</style>\n";
        html << "</head><body>\n";

        html << "<h1>Reporte de Compilación HULK</h1>\n";

        if (!errorHandler.hasErrors() && !errorHandler.hasWarnings())
        {
            html << "<div class='success'>✓ Compilación exitosa sin errores ni advertencias</div>\n";
        }
        else
        {
            if (errorHandler.hasErrors())
            {
                html << "<h2>Errores (" << errorHandler.getErrorCount() << ")</h2>\n";
            }
            if (errorHandler.hasWarnings())
            {
                html << "<h2>Advertencias (" << errorHandler.getWarningCount() << ")</h2>\n";
            }

            // Aquí deberías iterar sobre los errores del errorHandler
            // Por ahora, solo agregamos el reporte en texto plano
            std::string report = errorHandler.generateReport();
            html << "<pre>" << report << "</pre>\n";
        }

        html << "</body></html>\n";

        return html.str();
    }
};

// Macro para agregar contexto automáticamente a los errores
#define REPORT_ERROR_WITH_CONTEXT(type, msg, loc)                                 \
    do                                                                            \
    {                                                                             \
        std::string context = ErrorUtils::getLineContext(loc.filename, loc.line); \
        ErrorHandler::getInstance().reportError(type, msg, loc, context);         \
    } while (0)

#define REPORT_WARNING_WITH_CONTEXT(type, msg, loc)                               \
    do                                                                            \
    {                                                                             \
        std::string context = ErrorUtils::getLineContext(loc.filename, loc.line); \
        ErrorHandler::getInstance().reportWarning(type, msg, loc, context);       \
    } while (0)

#endif // ERROR_UTILS_HPP
