#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <variant>
#include <iostream>
#include <sstream>

// Valor en el intérprete CIL
using CILValue = std::variant<double, std::string, bool>;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

// Instrucción CIL
struct CILInstruction
{
    enum Type
    {
        ASSIGNMENT,     // x = y
        BINARY_OP,      // x = y + z
        CALL,           // x = CALL func arg1 arg2
        GETATTR,        // x = GETATTR obj attr
        SETATTR,        // SETATTR obj attr value
        GETINDEX,       // x = GETINDEX arr index
        SETINDEX,       // SETINDEX arr index value
        ALLOCATE,       // x = ALLOCATE type
        RETURN,         // RETURN value
        GOTO,           // GOTO label
        IF_GOTO,        // IF condition GOTO label
        LABEL           // label:
    };
    
    Type type;
    std::string dest;        // Variable destino
    std::string op;          // Operador o función
    std::vector<std::string> args; // Argumentos
    std::string label;       // Para GOTO y LABEL
};

// Función CIL
struct CILFunction
{
    std::string name;
    std::vector<std::string> args;
    std::vector<std::string> locals;
    std::vector<CILInstruction> instructions;
    int start_line = 0; // Línea donde comienza la función
};

// Marco de activación
struct ActivationFrame
{
    std::string function_name;
    std::unordered_map<std::string, CILValue> variables;
    int return_address = -1; // Dirección de retorno
    CILValue return_value; // Valor de retorno
    
    ActivationFrame() : return_value(0.0) {}
};

class CILInterpreter
{
private:
    // Datos del programa
    std::unordered_map<std::string, std::string> string_constants;
    std::unordered_map<std::string, CILFunction> functions;
    std::vector<CILInstruction> global_instructions;
    
    // Estado de ejecución
    std::stack<ActivationFrame> call_stack;
    ActivationFrame* current_frame = nullptr;
    int program_counter = 0;
    bool program_running = true;
    
    // Variables globales
    std::unordered_map<std::string, CILValue> globals;
    
public:
    CILInterpreter();
    
    // Cargar y ejecutar programa CIL
    void loadProgram(const std::string& cil_code);
    void execute();
    
    // Ejecutar función específica
    CILValue executeFunction(const std::string& func_name, const std::vector<CILValue>& args);
    
    // Métodos auxiliares
    void parseDataSection(const std::string& section);
    void parseCodeSection(const std::string& section);
    CILInstruction parseInstruction(const std::string& line);
    
    // Control de flujo
    int findLabel(const CILFunction& func, const std::string& label);
    bool shouldJump(const CILInstruction& instr);
    
    // Ejecución de instrucciones
    void executeInstruction(const CILInstruction& instr);
    void executeAssignment(const CILInstruction& instr);
    void executeBinaryOp(const CILInstruction& instr);
    void executeCall(const CILInstruction& instr);
    void executeReturn(const CILInstruction& instr);
    void executeGoto(const CILInstruction& instr);
    void executeIfGoto(const CILInstruction& instr);
    
    // Manejo de valores
    CILValue getValue(const std::string& name);
    void setValue(const std::string& name, const CILValue& value);
    bool isNumeric(const std::string& str);
    CILValue parseValue(const std::string& str);
    
    // Funciones nativas
    CILValue callNativeFunction(const std::string& name, const std::vector<CILValue>& args);
    
    // Utilidades
    void printValue(const CILValue& value);
    std::string valueToString(const CILValue& value);
    double valueToNumber(const CILValue& value);
    bool valueToBool(const CILValue& value);
};