#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "../AST/ast.hpp"

class CILGenerator : public StmtVisitor, public ExprVisitor
{
private:
    std::ostringstream data_section;
    std::ostringstream code_section;
    std::vector<std::string> string_constants;
    std::unordered_map<std::string, std::string> string_labels;
    
    int temp_counter = 0;
    int label_counter = 0;
    std::string current_function;
    std::vector<std::string> function_locals;
    std::vector<std::string> function_args;
    
    std::string last_temp; // Para almacenar el resultado de la última expresión
    
public:
    CILGenerator();
    
    // Método principal para generar código
    std::string generateCode(Program* program);
    
    // Métodos auxiliares
    std::string newTemp();
    std::string newLabel();
    std::string getStringLabel(const std::string& str);
    void emitInstruction(const std::string& instruction);
    void emitAssignment(const std::string& dest, const std::string& source);
    void emitBinaryOp(const std::string& dest, const std::string& left, 
                     const std::string& op, const std::string& right);
    
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
};