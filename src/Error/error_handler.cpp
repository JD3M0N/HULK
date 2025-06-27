#include "error_handler.hpp"
#include <algorithm>

std::string ErrorMessage::toString() const
{
    std::stringstream ss;

    // Formato: [SEVERITY] TYPE at location: message
    ss << "[" << getSeverityString() << "] " << getTypeString();

    if (!location.filename.empty() || location.line > 0)
    {
        ss << " at " << location.toString();
    }

    ss << ": " << message;

    if (!context.empty())
    {
        ss << "\n    Context: " << context;
    }

    return ss.str();
}

std::string ErrorMessage::getTypeString() const
{
    switch (type)
    {
    case ErrorType::LEXICAL_ERROR:
        return "Lexical Error";
    case ErrorType::SYNTAX_ERROR:
        return "Syntax Error";
    case ErrorType::SEMANTIC_ERROR:
        return "Semantic Error";
    case ErrorType::TYPE_ERROR:
        return "Type Error";
    case ErrorType::SCOPE_ERROR:
        return "Scope Error";
    case ErrorType::CODEGEN_ERROR:
        return "Code Generation Error";
    case ErrorType::RUNTIME_ERROR:
        return "Runtime Error";
    case ErrorType::INTERNAL_ERROR:
        return "Internal Compiler Error";
    default:
        return "Unknown Error";
    }
}

std::string ErrorMessage::getSeverityString() const
{
    switch (severity)
    {
    case ErrorSeverity::WARNING:
        return "WARNING";
    case ErrorSeverity::ERROR:
        return "ERROR";
    case ErrorSeverity::FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

void ErrorHandler::reportError(ErrorType type, const std::string &message,
                               const SourceLocation &location, const std::string &context)
{
    if (errorCount >= maxErrors)
    {
        return; // No agregar más errores si se alcanzó el límite
    }

    errors.emplace_back(type, ErrorSeverity::ERROR, message, location, context);
    errorCount++;

    if (errorCount >= maxErrors)
    {
        shouldAbort = true;
        std::cerr << "Error limit (" << maxErrors << ") exceeded. Aborting compilation.\n";
    }
}

void ErrorHandler::reportWarning(ErrorType type, const std::string &message,
                                 const SourceLocation &location, const std::string &context)
{
    errors.emplace_back(type, ErrorSeverity::WARNING, message, location, context);
    warningCount++;
}

void ErrorHandler::reportFatal(ErrorType type, const std::string &message,
                               const SourceLocation &location, const std::string &context)
{
    errors.emplace_back(type, ErrorSeverity::FATAL, message, location, context);
    errorCount++;
    shouldAbort = true;

    // Imprimir inmediatamente el error fatal
    std::cerr << "FATAL ERROR: " << errors.back().toString() << std::endl;
}

void ErrorHandler::printErrors(std::ostream &out) const
{
    for (const auto &error : errors)
    {
        out << error.toString() << std::endl;
    }
}

void ErrorHandler::printSummary(std::ostream &out) const
{
    if (hasErrors() || hasWarnings())
    {
        out << "\n=== COMPILATION SUMMARY ===" << std::endl;
        if (hasErrors())
        {
            out << "Errors: " << errorCount << std::endl;
        }
        if (hasWarnings())
        {
            out << "Warnings: " << warningCount << std::endl;
        }

        if (shouldAbortCompilation())
        {
            out << "Compilation FAILED due to errors." << std::endl;
        }
        else if (hasWarnings())
        {
            out << "Compilation completed with warnings." << std::endl;
        }
    }
    else
    {
        out << "Compilation completed successfully with no errors or warnings." << std::endl;
    }
}

std::string ErrorHandler::generateReport() const
{
    std::stringstream ss;

    // Separar errores por tipo
    std::vector<ErrorMessage> lexicalErrors, syntaxErrors, semanticErrors,
        typeErrors, scopeErrors, codegenErrors,
        runtimeErrors, internalErrors;

    for (const auto &error : errors)
    {
        switch (error.getType())
        {
        case ErrorType::LEXICAL_ERROR:
            lexicalErrors.push_back(error);
            break;
        case ErrorType::SYNTAX_ERROR:
            syntaxErrors.push_back(error);
            break;
        case ErrorType::SEMANTIC_ERROR:
            semanticErrors.push_back(error);
            break;
        case ErrorType::TYPE_ERROR:
            typeErrors.push_back(error);
            break;
        case ErrorType::SCOPE_ERROR:
            scopeErrors.push_back(error);
            break;
        case ErrorType::CODEGEN_ERROR:
            codegenErrors.push_back(error);
            break;
        case ErrorType::RUNTIME_ERROR:
            runtimeErrors.push_back(error);
            break;
        case ErrorType::INTERNAL_ERROR:
            internalErrors.push_back(error);
            break;
        }
    }

    auto printCategory = [&ss](const std::string &category, const std::vector<ErrorMessage> &errorList)
    {
        if (!errorList.empty())
        {
            ss << "\n=== " << category << " ===" << std::endl;
            for (const auto &error : errorList)
            {
                ss << error.toString() << std::endl;
            }
        }
    };

    printCategory("LEXICAL ERRORS", lexicalErrors);
    printCategory("SYNTAX ERRORS", syntaxErrors);
    printCategory("SEMANTIC ERRORS", semanticErrors);
    printCategory("TYPE ERRORS", typeErrors);
    printCategory("SCOPE ERRORS", scopeErrors);
    printCategory("CODE GENERATION ERRORS", codegenErrors);
    printCategory("RUNTIME ERRORS", runtimeErrors);
    printCategory("INTERNAL ERRORS", internalErrors);

    return ss.str();
}

void ErrorHandler::clear()
{
    errors.clear();
    errorCount = 0;
    warningCount = 0;
    shouldAbort = false;
}

// Métodos de conveniencia
void ErrorHandler::lexicalError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::LEXICAL_ERROR, message, loc, ctx);
}

void ErrorHandler::syntaxError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::SYNTAX_ERROR, message, loc, ctx);
}

void ErrorHandler::semanticError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::SEMANTIC_ERROR, message, loc, ctx);
}

void ErrorHandler::typeError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::TYPE_ERROR, message, loc, ctx);
}

void ErrorHandler::scopeError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::SCOPE_ERROR, message, loc, ctx);
}

void ErrorHandler::codegenError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::CODEGEN_ERROR, message, loc, ctx);
}

void ErrorHandler::runtimeError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportError(ErrorType::RUNTIME_ERROR, message, loc, ctx);
}

void ErrorHandler::internalError(const std::string &message, const SourceLocation &loc, const std::string &ctx)
{
    reportFatal(ErrorType::INTERNAL_ERROR, message, loc, ctx);
}

// Singleton implementation
ErrorHandler &ErrorHandler::getInstance()
{
    static ErrorHandler instance;
    return instance;
}
