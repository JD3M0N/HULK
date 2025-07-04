#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <sstream>
#include <memory>
#include <regex>

class MIPSGenerator
{
private:
    std::ostringstream data_section;
    std::ostringstream text_section;

    // Gestión de registros
    std::vector<std::string> temp_registers = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7"};
    std::vector<std::string> saved_registers = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"};
    std::unordered_map<std::string, std::string> variable_registers;
    int next_temp_reg = 0;

    // Gestión de memoria
    std::unordered_map<std::string, int> string_labels;
    std::unordered_map<std::string, int> variable_offsets;
    int stack_offset = 0;

    // Gestión de funciones
    std::unordered_map<std::string, int> function_labels;
    int label_counter = 0;
    int next_offset = 0; // ← NUEVO: contador de offsets por función

    // ← NUEVO: Gestión dinámica de métodos polimórficos
    std::unordered_map<std::string, std::vector<std::string>> method_to_functions; // método -> [f0, f1, ...]
    std::set<std::string> discovered_methods;                                      // métodos encontrados en VCALL

    // ← ARREGLO DEL BUG: Mapeo directo tipo -> método -> función
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> type_to_method_function;

    // ← NUEVO: Gestión dinámica de IDs de tipos
    std::unordered_map<std::string, int> type_ids; // tipo -> ID único
    int next_type_id = 1;                          // empezar en 1 (0 reservado para "desconocido")

    // ← NUEVO: Rastreo de tipos de temporales
    std::unordered_map<std::string, std::string> temporal_types; // temporal -> "int" | "string" | "object"

    // ← NUEVO MÉTODO AUXILIAR
    bool isLiteral(const std::string &var);

public:
    MIPSGenerator();

    // Método principal
    std::string generateMIPS(const std::string &cil_code);

    // Métodos auxiliares
    std::string getNextTempRegister();
    std::string getRegisterForVariable(const std::string &var);
    std::string newLabel();
    void emitInstruction(const std::string &instruction);
    void emitComment(const std::string &comment);

    // Procesamiento de secciones CIL
    void processDataSection(const std::string &section);
    void processTypesSection(const std::string &section);
    void processCodeSection(const std::string &section);

    // ← SIGNATURA SIMPLIFICADA
    void translateCILInstruction(const std::string &line,
                                 std::unordered_map<std::string, int> &local_vars);

    // ← SIGNATURAS CON STACK FRAMES
    void generateAssignment(const std::string &dest, const std::string &src,
                            const std::unordered_map<std::string, int> &local_vars);
    void generateBinaryOp(const std::string &dest, const std::string &left,
                          const std::string &op, const std::string &right,
                          const std::unordered_map<std::string, int> &local_vars);
    void generateFunctionCall(const std::string &dest, const std::string &func,
                              const std::vector<std::string> &args,
                              const std::unordered_map<std::string, int> &local_vars);
    void generatePrint(const std::string &value,
                       const std::unordered_map<std::string, int> &local_vars);
    void generateReturn(const std::string &value,
                        const std::unordered_map<std::string, int> &local_vars);
    void generateConditionalJump(const std::string &left, const std::string &op,
                                 const std::string &right, const std::string &label,
                                 const std::unordered_map<std::string, int> &local_vars);

    // Manejo de memoria y heap para objetos
    void generateObjectAllocation(const std::string &dest, const std::string &type);
    void generateArrayAllocation(const std::string &dest, const std::string &size);
    void generateGetAttribute(const std::string &dest, const std::string &obj, const std::string &attr);
    void generateSetAttribute(const std::string &obj, const std::string &attr, const std::string &value);

    // ← NUEVO: Gestión de IDs de tipos
    int getTypeId(const std::string &type);

    // ← NUEVO: Generación dinámica de dispatchers polimórficos
    void generatePolymorphicDispatchers(std::ostringstream &result);
};