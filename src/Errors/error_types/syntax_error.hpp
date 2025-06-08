#ifndef SYNTAX_ERROR_HPP
#define SYNTAX_ERROR_HPP

#include "../error.hpp"

class SyntaxError : public Error {
public:
    SyntaxError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error de Sintaxis"; 
    }
};

#endif // SYNTAX_ERROR_HPP