#ifndef CLASS_NODES_HPP
#define CLASS_NODES_HPP

#include "ast.hpp"
#include <vector>
#include <memory>
#include <string>

// Forward declarations para evitar dependencias circulares
class SemanticValidator;
class ITypeContext;

// Definición de clase: class Name inherits ParentName { attributes... methods... }
struct ClassDef : Stmt {
    std::string name;                           // Nombre de la clase
    std::string parent;                         // Clase padre (vacío si no hereda)
    std::vector<std::unique_ptr<AttrDef>> attributes;  // Lista de atributos
    std::vector<std::unique_ptr<MethodDef>> methods;   // Lista de métodos
    
    ClassDef(const std::string& className, 
             const std::string& parentName = "",
             std::vector<std::unique_ptr<AttrDef>>&& attrs = {},
             std::vector<std::unique_ptr<MethodDef>>&& meths = {})
        : name(className), parent(parentName), 
          attributes(std::move(attrs)), methods(std::move(meths)) {}
    
    void accept(StmtVisitor* v) override {
        v->visit(this);
    }
    
    // Declaración de validación semántica
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Definición de atributo: name : Type = initialization;
struct AttrDef : Stmt {
    std::string name;           // Nombre del atributo
    std::string type;           // Tipo del atributo (como string por ahora)
    ExprPtr initialization;     // Expresión de inicialización (puede ser nullptr)
    
    AttrDef(const std::string& attrName, 
            const std::string& attrType, 
            ExprPtr initExpr = nullptr)
        : name(attrName), type(attrType), initialization(std::move(initExpr)) {}
    
    void accept(StmtVisitor* v) override {
        v->visit(this);
    }
    
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Definición de método: methodName(arg1: Type1, arg2: Type2) : ReturnType => body
struct MethodDef : Stmt {
    std::string name;                       // Nombre del método
    std::string returnType;                 // Tipo de retorno
    std::vector<std::string> argNames;      // Nombres de los argumentos
    std::vector<std::string> argTypes;      // Tipos de los argumentos
    ExprPtr body;                          // Cuerpo del método (una expresión)
    
    MethodDef(const std::string& methodName,
              const std::string& retType,
              std::vector<std::string>&& paramNames,
              std::vector<std::string>&& paramTypes,
              ExprPtr methodBody)
        : name(methodName), returnType(retType),
          argNames(std::move(paramNames)), argTypes(std::move(paramTypes)),
          body(std::move(methodBody)) {}
    
    void accept(StmtVisitor* v) override {
        v->visit(this);
    }
    
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Expresión de creación de objeto: new ClassName(arg1, arg2, ...)
struct NewExpr : Expr {
    std::string className;              // Nombre de la clase a instanciar
    std::vector<ExprPtr> args;          // Argumentos para el constructor
    
    NewExpr(const std::string& clsName, std::vector<ExprPtr>&& arguments)
        : className(clsName), args(std::move(arguments)) {}
    
    void accept(ExprVisitor* v) override {
        v->visit(this);
    }
    
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Acceso a atributo: object.attribute
struct AttributeAccessExpr : Expr {
    ExprPtr object;                     // Expresión que evalúa al objeto
    std::string attribute;              // Nombre del atributo
    
    AttributeAccessExpr(ExprPtr obj, const std::string& attr)
        : object(std::move(obj)), attribute(attr) {}
    
    void accept(ExprVisitor* v) override {
        v->visit(this);
    }
    
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Llamada a método: object.method(arg1, arg2, ...)
struct MethodCallExpr : Expr {
    ExprPtr object;                     // Expresión que evalúa al objeto
    std::string method;                 // Nombre del método
    std::vector<ExprPtr> args;          // Argumentos de la llamada
    
    MethodCallExpr(ExprPtr obj, const std::string& methodName, std::vector<ExprPtr>&& arguments)
        : object(std::move(obj)), method(methodName), args(std::move(arguments)) {}
    
    void accept(ExprVisitor* v) override {
        v->visit(this);
    }
    
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Expresión 'self' para referenciar al objeto actual dentro de métodos
struct SelfExpr : Expr {
    SelfExpr() = default;
    
    void accept(ExprVisitor* v) override {
        v->visit(this);
    }
    
    bool validate(std::shared_ptr<IContext> context, SemanticValidator* validator = nullptr) override;
};

// Extensión de los visitors para soportar los nuevos nodos

// Agregar nuevas declaraciones a StmtVisitor
inline void extendStmtVisitor() {
    // Esta función es solo para documentar las extensiones necesarias
    // Las clases que implementen StmtVisitor deberán agregar:
    // virtual void visit(ClassDef*) = 0;
    // virtual void visit(AttrDef*) = 0;
    // virtual void visit(MethodDef*) = 0;
}

// Agregar nuevas declaraciones a ExprVisitor
inline void extendExprVisitor() {
    // Esta función es solo para documentar las extensiones necesarias
    // Las clases que implementen ExprVisitor deberán agregar:
    // virtual void visit(NewExpr*) = 0;
    // virtual void visit(AttributeAccessExpr*) = 0;
    // virtual void visit(MethodCallExpr*) = 0;
    // virtual void visit(SelfExpr*) = 0;
}

#endif // CLASS_NODES_HPP