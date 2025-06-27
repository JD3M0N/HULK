#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "../AST/ast.hpp"

class CILGenerator : public StmtVisitor, public ExprVisitor
{
private:
    std::ostringstream types_section;  // ← NUEVA SECCIÓN
    std::ostringstream data_section;
    std::ostringstream code_section;
    std::vector<std::string> string_constants;
    std::unordered_map<std::string, std::string> string_labels;
    
    int temp_counter = 0;
    int label_counter = 0;
    int function_counter = 0;  // ← CONTADOR DE FUNCIONES
    std::string current_function;
    std::vector<std::string> function_locals;
    std::vector<std::string> function_args;
    
    std::string last_temp; // Para almacenar el resultado de la última expresión
    
    // Nuevo: Para manejo de herencia
    std::vector<ClassDecl*> class_registry; // Registro de todas las clases procesadas
    
public:
    CILGenerator();
    
    // Método principal para generar código
    std::string generateCode(Program* program);
    
    // Métodos auxiliares
    std::string newTemp();
    std::string newLabel();
    std::string newFunctionLabel();  // ← NUEVO MÉTODO
    std::string getStringLabel(const std::string& str);
    void emitInstruction(const std::string& instruction);
    void emitAssignment(const std::string& dest, const std::string& source);
    void emitBinaryOp(const std::string& dest, const std::string& left, 
                     const std::string& op, const std::string& right);
    
    // Nuevos métodos para manejo de tipos
    void emitTypeDeclaration(const std::string& type_name, 
                           const std::vector<std::string>& attributes,
                           const std::vector<std::pair<std::string, std::string>>& methods);
    
    // ← NUEVO MÉTODO PARA HERENCIA
    void emitTypeDeclarationWithInheritance(
        const std::string& type_name,
        const std::string& parent_name, 
        const std::vector<std::string>& attributes,
        const std::vector<std::pair<std::string, std::string>>& methods);
    
    // StmtVisitor
    void visit(Program* prog) override;
    void visit(ExprStmt* stmt) override;
    void visit(FunctionDecl* func) override;
    void visit(ClassDecl* cls) override;
    
    // ExprVisitor
    void visit(NumberExpr* expr) override;
    void visit(StringExpr* expr) override;
    void visit(BooleanExpr* expr) override;
    void visit(UnaryExpr* expr) override;
    void visit(BinaryExpr* expr) override;
    void visit(CallExpr* expr) override;
    void visit(VariableExpr* expr) override;
    void visit(LetExpr* expr) override;
    void visit(AssignExpr* expr) override;
    void visit(IfExpr* expr) override;
    void visit(ExprBlock* expr) override;
    void visit(WhileExpr* expr) override;
    void visit(NewExpr* expr) override;
    void visit(SelfExpr* expr) override;
    void visit(BaseExpr* expr) override;
    void visit(MemberAccessExpr* expr) override;
    void visit(MemberAssignExpr* expr) override;
    void visit(MemberCallExpr* expr) override;
    
private:
    // Nuevos métodos auxiliares para herencia
    ClassDecl* findParentClass(const std::string& parent_name);
    void addAncestorAttributes(ClassDecl* parent_class, std::vector<std::string>& attr_names);
    void addAncestorMethods(ClassDecl* parent_class, std::vector<std::pair<std::string, std::string>>& method_info);
    void generateInheritedMethods(ClassDecl* parent_class, ClassDecl* current_class, 
                                 const std::vector<std::pair<std::string, std::string>>& method_info, 
                                 int& method_index);
    void registerClass(ClassDecl* cls);
};