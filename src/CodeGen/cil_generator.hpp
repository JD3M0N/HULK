#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "../AST/ast.hpp"

// Estructura para representar una VTable
struct VTableEntry
{
    std::string method_name;
    std::string function_label;
    int method_id;
};

struct VTable
{
    std::string class_name;
    std::vector<VTableEntry> entries;
    std::unordered_map<std::string, int> method_to_index; // Para búsqueda rápida
};

class CILGenerator : public StmtVisitor, public ExprVisitor
{
private:
    std::ostringstream types_section;
    std::ostringstream data_section;
    std::ostringstream code_section;
    std::vector<std::string> string_constants;
    std::unordered_map<std::string, std::string> string_labels;

    int temp_counter = 0;
    int label_counter = 0;
    int function_counter = 0;
    int method_id_counter = 0; // Para asignar IDs únicos a métodos

    std::string current_function;
    std::vector<std::string> function_locals;
    std::vector<std::string> function_args;

    std::string last_temp;

    // Sistema de VTable global
    std::unordered_map<std::string, VTable> vtables;         // class_name -> VTable
    std::unordered_map<int, std::string> method_id_to_label; // method_id -> function_label
    std::vector<ClassDecl *> class_registry;

public:
    CILGenerator();

    // Método principal para generar código
    std::string generateCode(Program *program);

    // Métodos auxiliares
    std::string newTemp();
    std::string newLabel();
    std::string newFunctionLabel();
    int newMethodId();
    std::string getStringLabel(const std::string &str);
    void emitInstruction(const std::string &instruction);
    void emitAssignment(const std::string &dest, const std::string &source);
    void emitBinaryOp(const std::string &dest, const std::string &left,
                      const std::string &op, const std::string &right);

    // Métodos para VTable
    void buildVTable(ClassDecl *cls);
    void emitVTableDeclaration(const std::string &class_name, const VTable &vtable);
    void emitMethodDispatchTable();

    // Métodos para manejo de tipos
    void emitTypeDeclaration(const std::string &type_name,
                             const std::vector<std::string> &attributes,
                             const std::vector<std::pair<std::string, std::string>> &methods);

    void emitTypeDeclarationWithInheritance(
        const std::string &type_name,
        const std::string &parent_name,
        const std::vector<std::string> &attributes,
        const std::vector<std::pair<std::string, std::string>> &methods);

    // StmtVisitor
    void visit(Program *prog) override;
    void visit(ExprStmt *stmt) override;
    void visit(FunctionDecl *func) override;
    void visit(ClassDecl *cls) override;

    // ExprVisitor
    void visit(NumberExpr *expr) override;
    void visit(StringExpr *expr) override;
    void visit(BooleanExpr *expr) override;
    void visit(UnaryExpr *expr) override;
    void visit(BinaryExpr *expr) override;
    void visit(CallExpr *expr) override;
    void visit(VariableExpr *expr) override;
    void visit(LetExpr *expr) override;
    void visit(AssignExpr *expr) override;
    void visit(IfExpr *expr) override;
    void visit(ExprBlock *expr) override;
    void visit(WhileExpr *expr) override;
    void visit(NewExpr *expr) override;
    void visit(SelfExpr *expr) override;
    void visit(BaseExpr *expr) override;
    void visit(MemberAccessExpr *expr) override;
    void visit(MemberAssignExpr *expr) override;

private:
    // Nuevos métodos auxiliares para herencia
    ClassDecl *findParentClass(const std::string &parent_name);
    void addAncestorAttributes(ClassDecl *parent_class, std::vector<std::string> &attr_names);
    void addAncestorMethods(ClassDecl *parent_class, std::vector<std::pair<std::string, std::string>> &method_info);
    void generateInheritedMethods(ClassDecl *parent_class, ClassDecl *current_class,
                                  const std::vector<std::pair<std::string, std::string>> &method_info,
                                  int &method_index);
    void registerClass(ClassDecl *cls);
};