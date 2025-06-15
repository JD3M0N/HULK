#include "mips_generator.hpp"
#include <iostream>
#include <sstream>
#include <regex>

MIPSGenerator::MIPSGenerator()
{
    // Inicializar secciones MIPS
    data_section << ".data\n";
    text_section << ".text\n";
    text_section << ".globl main\n\n";
}

std::string MIPSGenerator::generateMIPS(const std::string& cil_code)
{
    std::istringstream stream(cil_code);
    std::string line;
    bool in_data = false, in_types = false, in_code = false;
    std::string current_section;
    
    while (std::getline(stream, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line == ".TYPES") { in_types = true; in_data = false; in_code = false; continue; }
        if (line == ".DATA")  { in_data = true; in_types = false; in_code = false; continue; }
        if (line == ".CODE")  { in_code = true; in_data = false; in_types = false; continue; }
        
        if (in_types) {
            processTypesSection(line);
        } else if (in_data) {
            processDataSection(line);
        } else if (in_code) {
            current_section += line + "\n";
        }
    }
    
    if (!current_section.empty()) {
        processCodeSection(current_section);
    }
    
    // Construir resultado final
    std::ostringstream result;
    result << data_section.str() << "\n";
    result << text_section.str();
    
    return result.str();
}

void MIPSGenerator::processDataSection(const std::string& line)
{
    // Parsear: str0 = "Hello World";
    std::regex data_regex("(\\w+)\\s*=\\s*\"([^\"]*)\"\\s*;");
    std::smatch match;
    
    if (std::regex_match(line, match, data_regex)) {
        std::string label = match[1].str();
        std::string value = match[2].str();
        
        data_section << label << ": .asciiz \"" << value << "\"\n";
        string_labels[label] = string_labels.size();
    }
}

void MIPSGenerator::processTypesSection(const std::string& line)
{
    // Para MIPS, los tipos se manejan en tiempo de ejecución
}

void MIPSGenerator::processCodeSection(const std::string& section)
{
    std::istringstream stream(section);
    std::string line;
    std::string current_function;
    bool in_function = false;
    
    // ← NUEVO: Mapeo de variables a offsets en el stack frame
    std::unordered_map<std::string, int> local_vars;
    std::vector<std::string> args;
    int current_offset = -12; // Empezar después de $ra(-4) y $fp(-8)
    
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) continue;
        
        // Detectar funciones
        std::regex func_regex("function\\s+(\\w+)\\s*\\{");
        std::smatch match;
        
        if (std::regex_match(line, match, func_regex)) {
            current_function = match[1].str();
            in_function = true;
            
            // ← RESETEAR MAPEO DE VARIABLES
            local_vars.clear();
            args.clear();
            current_offset = -12;
            
            // Generar etiqueta de función
            if (current_function == "entry") {
                text_section << "main:\n";
            } else {
                text_section << current_function << ":\n";
            }
            
            emitComment("Function: " + current_function);
            continue;
        }
        
        // ← PROCESAR ARG PARA ASIGNAR OFFSETS
        if (in_function && line.find("ARG ") == 0) {
            std::string arg_name = line.substr(4);
            if (arg_name.back() == ';') arg_name.pop_back();
            
            args.push_back(arg_name);
            local_vars[arg_name] = current_offset;
            current_offset -= 4;
            continue;
        }
        
        if (line == "}") {
            if (in_function) {
                // ← GENERAR PRÓLOGO CORRECTO (al principio de función)
                int frame_size = (-current_offset) + 8; // +8 para $ra y $fp
                frame_size = (frame_size + 15) & ~15;   // Alinear a 16 bytes
                
                // Insertar prólogo al inicio de la función
                std::string function_body = text_section.str();
                size_t func_start = function_body.rfind(current_function == "entry" ? "main:" : current_function + ":");
                
                std::ostringstream prolog;
                prolog << "    # Prolog\n";
                prolog << "    addi $sp, $sp, -" << frame_size << "\n";
                prolog << "    sw $ra, " << (frame_size - 4) << "($sp)\n";
                prolog << "    sw $fp, " << (frame_size - 8) << "($sp)\n";
                prolog << "    move $fp, $sp\n";
                
                // ← CARGAR ARGUMENTOS DESDE REGISTROS A STACK
                for (size_t i = 0; i < args.size() && i < 4; i++) {
                    int offset = local_vars[args[i]];
                    prolog << "    sw $a" << i << ", " << (offset + frame_size) << "($sp)  # " << args[i] << "\n";
                }
                
                // Insertar prólogo
                if (func_start != std::string::npos) {
                    size_t insert_pos = function_body.find('\n', func_start) + 1;
                    function_body.insert(insert_pos, prolog.str());
                    text_section.str(function_body);
                    text_section.seekp(0, std::ios::end);
                }
                
                // ← GENERAR EPÍLOGO CORRECTO
                emitComment("Epilog");
                emitInstruction("lw $ra, " + std::to_string(frame_size - 4) + "($sp)");
                emitInstruction("lw $fp, " + std::to_string(frame_size - 8) + "($sp)");
                emitInstruction("addi $sp, $sp, " + std::to_string(frame_size));
                
                if (current_function == "entry") {
                    emitInstruction("li $v0, 10");  // Exit syscall
                    emitInstruction("syscall");
                } else {
                    emitInstruction("jr $ra");
                }
                
                text_section << "\n";
            }
            in_function = false;
            continue;
        }
        
        if (in_function) {
            translateCILInstruction(line, local_vars, current_offset);
        }
    }
}

// ← NUEVA FUNCIÓN CON MAPEO DE VARIABLES MEJORADO
void MIPSGenerator::translateCILInstruction(const std::string& line, 
                                          std::unordered_map<std::string, int>& local_vars,
                                          int& current_offset)
{
    std::string clean_line = line;
    if (!clean_line.empty() && clean_line.back() == ';')
        clean_line.pop_back();
    
    // ← FUNCIÓN MEJORADA PARA ASIGNAR OFFSET A NUEVAS VARIABLES
    auto ensureVariable = [&](const std::string& var) {
        // Verificar que no sea un literal numérico, string label, o ya existente
        if (local_vars.find(var) == local_vars.end() && 
            !isLiteral(var) && 
            string_labels.find(var) == string_labels.end()) {
            local_vars[var] = current_offset;
            current_offset -= 4;
            emitComment("Allocating variable: " + var + " at offset " + std::to_string(local_vars[var]));
        }
    };
    
    // ASSIGNMENT: t0 = 5 o t0 = t1
    std::regex assign_regex("(\\w+)\\s*=\\s*([\\w\\-\\.]+)");
    std::smatch match;
    
    if (std::regex_match(clean_line, match, assign_regex)) {
        std::string dest = match[1].str();
        std::string src = match[2].str();
        
        ensureVariable(dest);
        ensureVariable(src);
        
        generateAssignment(dest, src, local_vars);
        return;
    }
    
    // BINARY OP: t2 = t0 + t1
    std::regex binop_regex("(\\w+)\\s*=\\s*(\\w+)\\s*([+\\-*/<=!>]+)\\s*(\\w+)");
    if (std::regex_match(clean_line, match, binop_regex)) {
        std::string dest = match[1].str();
        std::string left = match[2].str();
        std::string op = match[3].str();
        std::string right = match[4].str();
        
        ensureVariable(dest);
        ensureVariable(left);
        ensureVariable(right);
        
        generateBinaryOp(dest, left, op, right, local_vars);
        return;
    }
    
    // CALL: t3 = CALL print t2
    std::regex call_regex("(\\w+)\\s*=\\s*CALL\\s+(\\w+)(.*)");
    if (std::regex_match(clean_line, match, call_regex)) {
        std::string dest = match[1].str();
        std::string func = match[2].str();
        std::string args_str = match[3].str();
        
        ensureVariable(dest);
        
        std::vector<std::string> args;
        std::istringstream iss(args_str);
        std::string arg;
        while (iss >> arg) {
            ensureVariable(arg);
            args.push_back(arg);
        }
        
        generateFunctionCall(dest, func, args, local_vars);
        return;
    }
    
    // RETURN: RETURN value
    if (clean_line.find("RETURN") == 0) {
        std::string value = "";
        if (clean_line.length() > 6) {
            value = clean_line.substr(7);
            ensureVariable(value);
        }
        generateReturn(value, local_vars);
        return;
    }
    
    // PRINT: PRINT value
    if (clean_line.find("PRINT") == 0) {
        std::string value = clean_line.substr(6);
        ensureVariable(value);
        generatePrint(value, local_vars);
        return;
    }
    
    // Etiquetas
    if (!clean_line.empty() && clean_line.back() == ':') {
        std::string label = clean_line.substr(0, clean_line.length() - 1);
        text_section << label << ":\n";
        return;
    }
    
    // GOTO
    if (clean_line.find("GOTO") == 0) {
        std::string label = clean_line.substr(5);
        emitInstruction("j " + label);
        return;
    }
    
    // IF GOTO: IF t0 == 0 GOTO L1
    std::regex if_goto_regex("IF\\s+(\\w+)\\s*(==|!=|<|>|<=|>=)\\s*(\\w+)\\s+GOTO\\s+(\\w+)");
    if (std::regex_match(clean_line, match, if_goto_regex)) {
        std::string left = match[1].str();
        std::string op = match[2].str();
        std::string right = match[3].str();
        std::string label = match[4].str();
        
        ensureVariable(left);
        ensureVariable(right);
        
        generateConditionalJump(left, op, right, label, local_vars);
        return;
    }
}

// ← FUNCIÓN AUXILIAR PARA DETECTAR LITERALES
bool MIPSGenerator::isLiteral(const std::string& var) {
    if (var.empty()) return false;
    
    // Verificar si es un número (entero o decimal)
    if (std::isdigit(var[0]) || (var[0] == '-' && var.length() > 1 && std::isdigit(var[1]))) {
        return true;
    }
    
    // Verificar si es un string literal (empieza con "str")
    if (var.find("str") == 0) {
        return true;
    }
    
    return false;
}

// ← NUEVAS IMPLEMENTACIONES CON MANEJO SEGURO DE VARIABLES
void MIPSGenerator::generateAssignment(const std::string& dest, const std::string& src,
                                     const std::unordered_map<std::string, int>& local_vars)
{
    // Si src es un número literal
    if (isLiteral(src)) {
        emitInstruction("li $t0, " + src.substr(0, src.find('.')));  // Truncar decimales para MIPS
        emitInstruction("sw $t0, " + std::to_string(local_vars.at(dest)) + "($fp)");
    }
    // Si src es una etiqueta de string
    else if (string_labels.find(src) != string_labels.end()) {
        emitInstruction("la $t0, " + src);
        emitInstruction("sw $t0, " + std::to_string(local_vars.at(dest)) + "($fp)");
    }
    // Si src es otra variable
    else if (local_vars.find(src) != local_vars.end()) {
        emitInstruction("lw $t0, " + std::to_string(local_vars.at(src)) + "($fp)");
        emitInstruction("sw $t0, " + std::to_string(local_vars.at(dest)) + "($fp)");
    }
    else {
        // Variable no encontrada, asignar 0
        emitComment("Warning: Variable " + src + " not found, assigning 0");
        emitInstruction("li $t0, 0");
        emitInstruction("sw $t0, " + std::to_string(local_vars.at(dest)) + "($fp)");
    }
}

void MIPSGenerator::generateBinaryOp(const std::string& dest, const std::string& left,
                                    const std::string& op, const std::string& right,
                                    const std::unordered_map<std::string, int>& local_vars)
{
    // Cargar operando izquierdo
    if (isLiteral(left)) {
        emitInstruction("li $t0, " + left.substr(0, left.find('.')));
    } else if (local_vars.find(left) != local_vars.end()) {
        emitInstruction("lw $t0, " + std::to_string(local_vars.at(left)) + "($fp)");
    } else {
        emitComment("Warning: Left operand " + left + " not found");
        emitInstruction("li $t0, 0");
    }
    
    // Cargar operando derecho
    if (isLiteral(right)) {
        emitInstruction("li $t1, " + right.substr(0, right.find('.')));
    } else if (local_vars.find(right) != local_vars.end()) {
        emitInstruction("lw $t1, " + std::to_string(local_vars.at(right)) + "($fp)");
    } else {
        emitComment("Warning: Right operand " + right + " not found");
        emitInstruction("li $t1, 0");
    }
    
    // Generar operación
    if (op == "+") {
        emitInstruction("add $t2, $t0, $t1");
    } else if (op == "-") {
        emitInstruction("sub $t2, $t0, $t1");
    } else if (op == "*") {
        emitInstruction("mul $t2, $t0, $t1");
    } else if (op == "/") {
        emitInstruction("div $t0, $t1");
        emitInstruction("mflo $t2");
    } else if (op == "<=") {
        emitInstruction("sle $t2, $t0, $t1");
    } else if (op == "<") {
        emitInstruction("slt $t2, $t0, $t1");
    } else if (op == ">=") {
        emitInstruction("sge $t2, $t0, $t1");
    } else if (op == ">") {
        emitInstruction("sgt $t2, $t0, $t1");
    } else if (op == "==") {
        emitInstruction("seq $t2, $t0, $t1");
    } else if (op == "!=") {
        emitInstruction("sne $t2, $t0, $t1");
    }
    
    // Almacenar resultado
    emitInstruction("sw $t2, " + std::to_string(local_vars.at(dest)) + "($fp)");
}

void MIPSGenerator::generateFunctionCall(const std::string& dest, const std::string& func,
                                       const std::vector<std::string>& args,
                                       const std::unordered_map<std::string, int>& local_vars)
{
    // Pasar argumentos en registros $a0-$a3
    for (size_t i = 0; i < args.size() && i < 4; i++) {
        if (isLiteral(args[i])) {
            emitInstruction("li $a" + std::to_string(i) + ", " + args[i].substr(0, args[i].find('.')));
        } else if (local_vars.find(args[i]) != local_vars.end()) {
            emitInstruction("lw $a" + std::to_string(i) + ", " + 
                          std::to_string(local_vars.at(args[i])) + "($fp)");
        } else if (string_labels.find(args[i]) != string_labels.end()) {
            emitInstruction("la $a" + std::to_string(i) + ", " + args[i]);
        } else {
            emitComment("Warning: Argument " + args[i] + " not found");
            emitInstruction("li $a" + std::to_string(i) + ", 0");
        }
    }
    
    // Llamadas a funciones nativas
    if (func == "print") {
        generatePrint(args.empty() ? "" : args[0], local_vars);
    } else {
        // Llamada a función definida por usuario
        emitInstruction("jal " + func);
        
        // Mover resultado a variable destino
        if (!dest.empty()) {
            emitInstruction("sw $v0, " + std::to_string(local_vars.at(dest)) + "($fp)");
        }
    }
}

void MIPSGenerator::generatePrint(const std::string& value,
                                const std::unordered_map<std::string, int>& local_vars)
{
    // Verificar si es string o número
    if (string_labels.find(value) != string_labels.end()) {
        // Imprimir string
        emitInstruction("la $a0, " + value);
        emitInstruction("li $v0, 4");  // print_str syscall
        emitInstruction("syscall");
    } else {
        // Cargar valor y imprimir entero
        if (isLiteral(value)) {
            emitInstruction("li $a0, " + value.substr(0, value.find('.')));
        } else if (local_vars.find(value) != local_vars.end()) {
            emitInstruction("lw $a0, " + std::to_string(local_vars.at(value)) + "($fp)");
        } else {
            emitComment("Warning: Print value " + value + " not found");
            emitInstruction("li $a0, 0");
        }
        emitInstruction("li $v0, 1");  // print_int syscall
        emitInstruction("syscall");
    }
    
    // Imprimir nueva línea
    emitInstruction("li $a0, 10");  // ASCII newline
    emitInstruction("li $v0, 11");  // print_char syscall
    emitInstruction("syscall");
}

void MIPSGenerator::generateReturn(const std::string& value,
                                 const std::unordered_map<std::string, int>& local_vars)
{
    if (!value.empty()) {
        if (isLiteral(value)) {
            emitInstruction("li $v0, " + value.substr(0, value.find('.')));
        } else if (local_vars.find(value) != local_vars.end()) {
            emitInstruction("lw $v0, " + std::to_string(local_vars.at(value)) + "($fp)");
        } else {
            emitComment("Warning: Return value " + value + " not found");
            emitInstruction("li $v0, 0");
        }
    }
}

void MIPSGenerator::generateConditionalJump(const std::string& left, const std::string& op, 
                                           const std::string& right, const std::string& label,
                                           const std::unordered_map<std::string, int>& local_vars)
{
    // Cargar operando izquierdo
    if (isLiteral(left)) {
        emitInstruction("li $t0, " + left.substr(0, left.find('.')));
    } else if (local_vars.find(left) != local_vars.end()) {
        emitInstruction("lw $t0, " + std::to_string(local_vars.at(left)) + "($fp)");
    } else {
        emitComment("Warning: Left operand " + left + " not found");
        emitInstruction("li $t0, 0");
    }
    
    // Cargar operando derecho
    if (isLiteral(right)) {
        emitInstruction("li $t1, " + right.substr(0, right.find('.')));
    } else if (local_vars.find(right) != local_vars.end()) {
        emitInstruction("lw $t1, " + std::to_string(local_vars.at(right)) + "($fp)");
    } else {
        emitComment("Warning: Right operand " + right + " not found");
        emitInstruction("li $t1, 0");
    }
    
    // Generar salto condicional
    if (op == "==") {
        emitInstruction("beq $t0, $t1, " + label);
    } else if (op == "!=") {
        emitInstruction("bne $t0, $t1, " + label);
    } else if (op == "<") {
        emitInstruction("blt $t0, $t1, " + label);
    } else if (op == "<=") {
        emitInstruction("ble $t0, $t1, " + label);
    } else if (op == ">") {
        emitInstruction("bgt $t0, $t1, " + label);
    } else if (op == ">=") {
        emitInstruction("bge $t0, $t1, " + label);
    }
}

// ← MÉTODOS AUXILIARES ACTUALIZADOS
std::string MIPSGenerator::getRegisterForVariable(const std::string& var)
{
    // Esta función ya no se usa con el nuevo enfoque de stack frames
    return "$t0";
}

std::string MIPSGenerator::getNextTempRegister()
{
    if (next_temp_reg < static_cast<int>(temp_registers.size())) {
        return temp_registers[next_temp_reg++];
    }
    return "$t0";
}

void MIPSGenerator::emitInstruction(const std::string& instruction)
{
    text_section << "    " << instruction << "\n";
}

void MIPSGenerator::emitComment(const std::string& comment)
{
    text_section << "    # " << comment << "\n";
}