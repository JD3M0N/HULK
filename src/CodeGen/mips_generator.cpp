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
    // Aquí podríamos generar tablas de métodos virtuales si fuera necesario
}

void MIPSGenerator::processCodeSection(const std::string& section)
{
    std::istringstream stream(section);
    std::string line;
    std::string current_function;
    bool in_function = false;
    
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
            
            // Generar etiqueta de función
            if (current_function == "entry") {
                text_section << "main:\n";
            } else {
                text_section << current_function << ":\n";
            }
            
            // Prólogo de función
            emitComment("Function: " + current_function);
            emitInstruction("addi $sp, $sp, -32");  // Reservar espacio en stack
            emitInstruction("sw $ra, 28($sp)");     // Guardar return address
            emitInstruction("sw $fp, 24($sp)");     // Guardar frame pointer
            emitInstruction("move $fp, $sp");       // Establecer nuevo frame pointer
            
            continue;
        }
        
        if (line == "}") {
            if (in_function) {
                // Epílogo de función
                emitInstruction("lw $ra, 28($sp)");
                emitInstruction("lw $fp, 24($sp)");
                emitInstruction("addi $sp, $sp, 32");
                
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
            translateCILInstruction(line);
        }
    }
}

void MIPSGenerator::translateCILInstruction(const std::string& line)
{
    std::string clean_line = line;
    if (!clean_line.empty() && clean_line.back() == ';')
        clean_line.pop_back();
    
    // ASSIGNMENT: t0 = 5
    std::regex assign_regex("(\\w+)\\s*=\\s*([\\w\\-\\.]+)");
    std::smatch match;
    
    if (std::regex_match(clean_line, match, assign_regex)) {
        std::string dest = match[1].str();
        std::string src = match[2].str();
        generateAssignment(dest, src);
        return;
    }
    
    // BINARY OP: t2 = t0 + t1
    std::regex binop_regex("(\\w+)\\s*=\\s*(\\w+)\\s*([+\\-*/])\\s*(\\w+)");
    if (std::regex_match(clean_line, match, binop_regex)) {
        std::string dest = match[1].str();
        std::string left = match[2].str();
        std::string op = match[3].str();
        std::string right = match[4].str();
        generateBinaryOp(dest, left, op, right);
        return;
    }
    
    // CALL: t3 = CALL print t2
    std::regex call_regex("(\\w+)\\s*=\\s*CALL\\s+(\\w+)(.*)");
    if (std::regex_match(clean_line, match, call_regex)) {
        std::string dest = match[1].str();
        std::string func = match[2].str();
        std::string args_str = match[3].str();
        
        std::vector<std::string> args;
        std::istringstream iss(args_str);
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        
        generateFunctionCall(dest, func, args);
        return;
    }
    
    // RETURN: RETURN value
    if (clean_line.find("RETURN") == 0) {
        std::string value = "";
        if (clean_line.length() > 6) {
            value = clean_line.substr(7);
        }
        generateReturn(value);
        return;
    }
    
    // PRINT: PRINT value
    if (clean_line.find("PRINT") == 0) {
        std::string value = clean_line.substr(6);
        generatePrint(value);
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
    
    // IF GOTO
    std::regex if_goto_regex("IF\\s+(\\w+)\\s*(==|!=)\\s*(\\w+)\\s+GOTO\\s+(\\w+)");
    if (std::regex_match(clean_line, match, if_goto_regex)) {
        std::string left = match[1].str();
        std::string op = match[2].str();
        std::string right = match[3].str();
        std::string label = match[4].str();
        
        std::string left_reg = getRegisterForVariable(left);
        std::string right_reg = getRegisterForVariable(right);
        
        if (op == "==") {
            emitInstruction("beq " + left_reg + ", " + right_reg + ", " + label);
        } else if (op == "!=") {
            emitInstruction("bne " + left_reg + ", " + right_reg + ", " + label);
        }
        return;
    }
}

void MIPSGenerator::generateAssignment(const std::string& dest, const std::string& src)
{
    std::string dest_reg = getRegisterForVariable(dest);
    
    // Si src es un número literal
    if (std::isdigit(src[0]) || (src[0] == '-' && std::isdigit(src[1]))) {
        emitInstruction("li " + dest_reg + ", " + src);
    }
    // Si src es una etiqueta de string
    else if (string_labels.find(src) != string_labels.end()) {
        emitInstruction("la " + dest_reg + ", " + src);
    }
    // Si src es otra variable
    else {
        std::string src_reg = getRegisterForVariable(src);
        emitInstruction("move " + dest_reg + ", " + src_reg);
    }
}

void MIPSGenerator::generateBinaryOp(const std::string& dest, const std::string& left,
                                    const std::string& op, const std::string& right)
{
    std::string dest_reg = getRegisterForVariable(dest);
    std::string left_reg = getRegisterForVariable(left);
    std::string right_reg = getRegisterForVariable(right);
    
    if (op == "+") {
        emitInstruction("add " + dest_reg + ", " + left_reg + ", " + right_reg);
    } else if (op == "-") {
        emitInstruction("sub " + dest_reg + ", " + left_reg + ", " + right_reg);
    } else if (op == "*") {
        emitInstruction("mul " + dest_reg + ", " + left_reg + ", " + right_reg);
    } else if (op == "/") {
        emitInstruction("div " + left_reg + ", " + right_reg);
        emitInstruction("mflo " + dest_reg);
    }
}

void MIPSGenerator::generateFunctionCall(const std::string& dest, const std::string& func,
                                       const std::vector<std::string>& args)
{
    // Pasar argumentos en registros $a0-$a3
    for (size_t i = 0; i < args.size() && i < 4; i++) {
        std::string arg_reg = getRegisterForVariable(args[i]);
        emitInstruction("move $a" + std::to_string(i) + ", " + arg_reg);
    }
    
    // Llamadas a funciones nativas
    if (func == "print") {
        generatePrint(args.empty() ? "" : args[0]);
    } else {
        // Llamada a función definida por usuario
        emitInstruction("jal " + func);
        
        // Mover resultado a registro destino
        if (!dest.empty()) {
            std::string dest_reg = getRegisterForVariable(dest);
            emitInstruction("move " + dest_reg + ", $v0");
        }
    }
}

void MIPSGenerator::generatePrint(const std::string& value)
{
    std::string value_reg = getRegisterForVariable(value);
    
    // Verificar si es string o número
    if (string_labels.find(value) != string_labels.end()) {
        // Imprimir string
        emitInstruction("move $a0, " + value_reg);
        emitInstruction("li $v0, 4");  // print_str syscall
        emitInstruction("syscall");
    } else {
        // Imprimir entero
        emitInstruction("move $a0, " + value_reg);
        emitInstruction("li $v0, 1");  // print_int syscall
        emitInstruction("syscall");
    }
    
    // Imprimir nueva línea
    emitInstruction("li $a0, 10");  // ASCII newline
    emitInstruction("li $v0, 11");  // print_char syscall
    emitInstruction("syscall");
}

void MIPSGenerator::generateReturn(const std::string& value)
{
    if (!value.empty()) {
        std::string value_reg = getRegisterForVariable(value);
        emitInstruction("move $v0, " + value_reg);
    }
}

std::string MIPSGenerator::getRegisterForVariable(const std::string& var)
{
    if (variable_registers.find(var) == variable_registers.end()) {
        // Asignar nuevo registro temporal
        if (next_temp_reg < static_cast<int>(temp_registers.size())) {  
            variable_registers[var] = temp_registers[next_temp_reg++];
        } else {
            // Si no hay más registros temporales, usar stack
            variable_registers[var] = "stack_" + std::to_string(stack_offset);
            stack_offset += 4;
        }
    }
    return variable_registers[var];
}

std::string MIPSGenerator::getNextTempRegister()
{
    if (next_temp_reg < static_cast<int>(temp_registers.size())) {  // ← AGREGAR CAST
        return temp_registers[next_temp_reg++];
    }
    return "$t0"; // Fallback
}

void MIPSGenerator::emitInstruction(const std::string& instruction)
{
    text_section << "    " << instruction << "\n";
}

void MIPSGenerator::emitComment(const std::string& comment)
{
    text_section << "    # " << comment << "\n";
}