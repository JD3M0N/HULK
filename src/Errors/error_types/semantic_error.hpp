#ifndef SEMANTIC_ERROR_HPP
#define SEMANTIC_ERROR_HPP

#include "../error.hpp"


class SemanticError : public Error {
public:
    SemanticError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error Semantico"; 
    }
};

#endif