#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>  
#include "error.hpp"

class ErrorHandler {
private:
    std::vector<std::shared_ptr<Error>> errors;
    bool abortOnError;
    ErrorLevel minReportLevel;
    std::function<void(const Error&)> errorCallback;
    
public:
    ErrorHandler(bool abort = false, 
                 ErrorLevel minLevel = ErrorLevel::WARNING,
                 std::function<void(const Error&)> callback = nullptr) 
        : abortOnError(abort), minReportLevel(minLevel), errorCallback(callback) {}
    
    void reportError(std::shared_ptr<Error> error) {
        errors.push_back(error);
        
        if (error->getLevel() >= minReportLevel && errorCallback) {
            errorCallback(*error);
        }
        
        if (abortOnError && error->getLevel() >= ErrorLevel::ERROR) {
            throw *error; // Podrías implementar un mecanismo más sofisticado
        }
    }
    
    template<typename ErrorType, typename... Args>
    void report(Args&&... args) {
        auto error = std::make_shared<ErrorType>(std::forward<Args>(args)...);
        reportError(error);
    }
    
    bool hasErrors() const {
        return std::any_of(errors.begin(), errors.end(), 
                          [](const auto& err) { 
                              return err->getLevel() >= ErrorLevel::ERROR; 
                          });
    }
    
    const std::vector<std::shared_ptr<Error>>& getErrors() const {
        return errors;
    }
    
    void clear() {
        errors.clear();
    }
};

// Singleton para acceso global
class ErrorManager {
private:
    inline static ErrorHandler* instance = nullptr;  // 'inline' permite múltiples definiciones
    
public:
    static ErrorHandler& getInstance() {
        if (!instance) {
            instance = new ErrorHandler();
        }
        return *instance;
    }
    
    static void initialize(bool abort = false, 
                          ErrorLevel minLevel = ErrorLevel::WARNING,
                          std::function<void(const Error&)> callback = nullptr) {
        if (instance) {
            delete instance;
        }
        instance = new ErrorHandler(abort, minLevel, callback);
    }
    
    static void destroy() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }
};

#endif // ERROR_HANDLER_HPP