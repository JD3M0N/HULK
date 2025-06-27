#ifndef COMPILATION_EXCEPTION_HPP
#define COMPILATION_EXCEPTION_HPP

#include <exception>
#include <string>
#include "error_handler.hpp"

class CompilationException : public std::exception
{
private:
    ErrorType errorType;
    std::string message;
    SourceLocation location;

public:
    CompilationException(ErrorType type, const std::string &msg,
                         const SourceLocation &loc = SourceLocation())
        : errorType(type), message(msg), location(loc) {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }

    ErrorType getErrorType() const { return errorType; }
    const SourceLocation &getLocation() const { return location; }

    std::string getDetailedMessage() const
    {
        std::string result = "CompilationException: ";
        result += message;
        if (!location.filename.empty() || location.line > 0)
        {
            result += " at " + location.toString();
        }
        return result;
    }
};

// Macros para lanzar excepciones específicas
#define THROW_LEXICAL_ERROR(msg, loc) throw CompilationException(ErrorType::LEXICAL_ERROR, msg, loc)
#define THROW_SYNTAX_ERROR(msg, loc) throw CompilationException(ErrorType::SYNTAX_ERROR, msg, loc)
#define THROW_SEMANTIC_ERROR(msg, loc) throw CompilationException(ErrorType::SEMANTIC_ERROR, msg, loc)
#define THROW_TYPE_ERROR(msg, loc) throw CompilationException(ErrorType::TYPE_ERROR, msg, loc)
#define THROW_SCOPE_ERROR(msg, loc) throw CompilationException(ErrorType::SCOPE_ERROR, msg, loc)
#define THROW_CODEGEN_ERROR(msg, loc) throw CompilationException(ErrorType::CODEGEN_ERROR, msg, loc)
#define THROW_RUNTIME_ERROR(msg, loc) throw CompilationException(ErrorType::RUNTIME_ERROR, msg, loc)
#define THROW_INTERNAL_ERROR(msg, loc) throw CompilationException(ErrorType::INTERNAL_ERROR, msg, loc)

#endif // COMPILATION_EXCEPTION_HPP
