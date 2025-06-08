#ifndef VALUE_ERROR_HPP
#define VALUE_ERROR_HPP

#include "../error.hpp"

class ValueError : public Error {
public:
    ValueError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error de Valor"; 
    }
};

#endif // VALUE_ERROR_HPP