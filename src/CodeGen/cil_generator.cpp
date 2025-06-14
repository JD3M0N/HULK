#include "cil_generator.hpp"
#include <iostream>
#include <iomanip>

CILGenerator::CILGenerator()
{
    // Inicializar secciones en el orden correcto
    types_section << ".TYPES\n";  // ← NUEVA SECCIÓN PRIMERO
    data_section << ".DATA\n";
    code_section << ".CODE\n";
}

std::string CILGenerator::generateCode(Program* program)
{
    // Generar código para el programa
    program->accept(this);
    
    // Construir el resultado final CON SECCIÓN DE TIPOS
    std::ostringstream result;
    result << types_section.str() << "\n";  // ← TIPOS PRIMERO
    result << data_section.str() << "\n";
    result << code_section.str();
    
    return result.str();
}

std::string CILGenerator::newTemp()
{
    return "t" + std::to_string(temp_counter++);
}

std::string CILGenerator::newLabel()
{
    return "L" + std::to_string(label_counter++);
}

std::string CILGenerator::newFunctionLabel()
{
    return "f" + std::to_string(function_counter++);
}

std::string CILGenerator::getStringLabel(const std::string& str)
{
    if (string_labels.find(str) == string_labels.end())
    {
        std::string label = "str" + std::to_string(string_constants.size());
        string_labels[str] = label;
        string_constants.push_back(str);
        
        // Agregar a la sección de datos
        data_section << label << " = \"" << str << "\";\n";
    }
    return string_labels[str];
}

void CILGenerator::emitInstruction(const std::string& instruction)
{
    code_section << "    " << instruction << ";\n";
}

void CILGenerator::emitAssignment(const std::string& dest, const std::string& source)
{
    emitInstruction(dest + " = " + source);
}

void CILGenerator::emitBinaryOp(const std::string& dest, const std::string& left,
                                const std::string& op, const std::string& right)
{
    emitInstruction(dest + " = " + left + " " + op + " " + right);
}

void CILGenerator::emitTypeDeclaration(const std::string& type_name, 
                                     const std::vector<std::string>& attributes,
                                     const std::vector<std::pair<std::string, std::string>>& methods)
{
    types_section << "type " << type_name << " {\n";
    
    // Emitir atributos
    for (const auto& attr : attributes)
    {
        types_section << "    attribute " << type_name << "_" << attr << " ;\n";
    }
    
    // Emitir métodos
    for (const auto& method : methods)
    {
        types_section << "    method " << type_name << "_" << method.first << ": " 
                     << method.second << " ;\n";
    }
    
    types_section << "}\n";
}

// ============ StmtVisitor ============

void CILGenerator::visit(Program* prog)
{
    // Primero generar todas las clases (que incluyen los tipos)
    for (auto& stmt : prog->stmts)
    {
        auto* cls = dynamic_cast<ClassDecl*>(stmt.get());
        if (cls)
        {
            cls->accept(this);
        }
    }
    
    // Luego generar todas las funciones
    for (auto& stmt : prog->stmts)
    {
        auto* func = dynamic_cast<FunctionDecl*>(stmt.get());
        if (func)
        {
            func->accept(this);
        }
    }
    
    // Crear función entry para el código restante (según especificación del libro)
    std::vector<StmtPtr> main_stmts;
    for (auto& stmt : prog->stmts)
    {
        auto* func = dynamic_cast<FunctionDecl*>(stmt.get());
        auto* cls = dynamic_cast<ClassDecl*>(stmt.get());
        if (!func && !cls) // No es función ni clase
        {
            // No podemos hacer release() aquí porque el stmt todavía está en uso
            // En su lugar, simplemente procesamos las expresiones directamente
        }
    }
    
    // Verificar si hay código no perteneciente a funciones o clases
    bool hasMainCode = false;
    for (auto& stmt : prog->stmts)
    {
        auto* func = dynamic_cast<FunctionDecl*>(stmt.get());
        auto* cls = dynamic_cast<ClassDecl*>(stmt.get());
        if (!func && !cls)
        {
            hasMainCode = true;
            break;
        }
    }
    
    if (hasMainCode)
    {
        code_section << "\nfunction entry {\n";  // ← CAMBIAR A "entry"
        current_function = "entry";
        
        // Procesar statements del main
        for (auto& stmt : prog->stmts)
        {
            auto* func = dynamic_cast<FunctionDecl*>(stmt.get());
            auto* cls = dynamic_cast<ClassDecl*>(stmt.get());
            if (!func && !cls)
            {
                stmt->accept(this);
            }
        }
        
        emitInstruction("RETURN 0");  // ← RETURN EXPLÍCITO
        code_section << "}\n";
    }
}

void CILGenerator::visit(ExprStmt* stmt)
{
    stmt->expr->accept(this);
    // El resultado está en last_temp, pero para ExprStmt no necesitamos hacer nada más
}

void CILGenerator::visit(FunctionDecl* func)
{
    current_function = func->name;
    function_locals.clear();
    function_args.clear();
    
    code_section << "\nfunction " << func->name << " {\n";
    
    // Declarar argumentos
    for (const auto& param : func->params)
    {
        code_section << "    ARG " << param << ";\n";
        function_args.push_back(param);
    }
    
    // Generar código del cuerpo
    func->body->accept(this);
    
    // Si la función devuelve algo, usar RETURN
    if (!last_temp.empty())
    {
        emitInstruction("RETURN " + last_temp);
    }
    
    code_section << "}\n";
}

void CILGenerator::visit(ClassDecl* cls)
{
    // 1. Generar declaración de tipo
    std::vector<std::string> attr_names;
    std::vector<std::pair<std::string, std::string>> method_info;
    
    // Recopilar nombres de atributos
    for (const auto& attr : cls->attributes)
    {
        attr_names.push_back(attr.first);
    }
    
    // Recopilar información de métodos
    for (const auto& method : cls->methods)
    {
        auto* func = dynamic_cast<FunctionDecl*>(method.get());
        if (func)
        {
            std::string function_label = newFunctionLabel();
            method_info.push_back({func->name, function_label});
        }
    }
    
    // Emitir declaración de tipo
    emitTypeDeclaration(cls->name, attr_names, method_info);
    
    // 2. Generar métodos como funciones separadas
    int method_index = 0;
    for (auto& method : cls->methods)
    {
        auto* func = dynamic_cast<FunctionDecl*>(method.get());
        if (func)
        {
            // Crear función con prefijo de clase y usar la etiqueta generada
            std::string old_name = func->name;
            std::string method_label = method_info[method_index].second;
            
            code_section << "\nfunction " << method_label << " {\n";
            current_function = method_label;
            
            // Agregar 'self' como primer parámetro
            code_section << "    PARAM self;\n";
            
            // Agregar parámetros del método
            for (const auto& param : func->params)
            {
                code_section << "    ARG " << param << ";\n";
            }
            
            // Generar código del cuerpo
            func->body->accept(this);
            
            // Return
            if (!last_temp.empty())
            {
                emitInstruction("RETURN " + last_temp);
            }
            else
            {
                emitInstruction("RETURN self");  // Retornar self por defecto
            }
            
            code_section << "}\n";
            
            method_index++;
        }
    }
}

// ============ ExprVisitor ============
// (El resto de métodos se mantienen igual)

void CILGenerator::visit(NumberExpr* expr)
{
    last_temp = std::to_string(expr->value);
}

void CILGenerator::visit(StringExpr* expr)
{
    last_temp = getStringLabel(expr->value);
}

void CILGenerator::visit(BooleanExpr* expr)
{
    last_temp = expr->value ? "1" : "0";
}

void CILGenerator::visit(UnaryExpr* expr)
{
    expr->operand->accept(this);
    std::string operand = last_temp;
    
    last_temp = newTemp();
    
    switch (expr->op)
    {
    case UnaryExpr::OP_NEG:
        emitBinaryOp(last_temp, "0", "-", operand);
        break;
    }
}

void CILGenerator::visit(BinaryExpr* expr)
{
    expr->left->accept(this);
    std::string left = last_temp;
    
    expr->right->accept(this);
    std::string right = last_temp;
    
    last_temp = newTemp();
    
    std::string op;
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD: op = "+"; break;
    case BinaryExpr::OP_SUB: op = "-"; break;
    case BinaryExpr::OP_MUL: op = "*"; break;
    case BinaryExpr::OP_DIV: op = "/"; break;
    case BinaryExpr::OP_MOD: op = "%"; break;
    case BinaryExpr::OP_POW: op = "^"; break;
    case BinaryExpr::OP_LT:  op = "<"; break;
    case BinaryExpr::OP_GT:  op = ">"; break;
    case BinaryExpr::OP_LE:  op = "<="; break;
    case BinaryExpr::OP_GE:  op = ">="; break;
    case BinaryExpr::OP_EQ:  op = "=="; break;
    case BinaryExpr::OP_NEQ: op = "!="; break;
    case BinaryExpr::OP_AND: op = "&&"; break;
    case BinaryExpr::OP_OR:  op = "||"; break;
    case BinaryExpr::OP_CONCAT: op = "@"; break;
    }
    
    emitBinaryOp(last_temp, left, op, right);
}

void CILGenerator::visit(CallExpr* expr)
{
    // Evaluar argumentos
    std::vector<std::string> args;
    for (auto& arg : expr->args)
    {
        arg->accept(this);
        args.push_back(last_temp);
    }
    
    last_temp = newTemp();
    
    // Emitir llamada a función
    std::string call_instr = last_temp + " = CALL " + expr->callee;
    for (const auto& arg : args)
    {
        call_instr += " " + arg;
    }
    
    emitInstruction(call_instr);
}

void CILGenerator::visit(VariableExpr* expr)
{
    last_temp = expr->name;
}

void CILGenerator::visit(LetExpr* expr)
{
    // Generar código según la plantilla del libro:
    // <init.code>
    // <var> = <init.value> ;
    // <body.code>
    // <value> = <body.value> ;
    
    // 1. Generar código del inicializador
    expr->initializer->accept(this);
    std::string init_value = last_temp;
    
    // 2. Asignar a la variable local
    emitAssignment(expr->name, init_value);
    
    // 3. Generar código del cuerpo
    expr->body->accept(this);
    
    // 4. El resultado del let es el resultado del cuerpo (ya está en last_temp)
    // No necesitamos hacer nada más porque last_temp ya contiene el valor correcto
}

void CILGenerator::visit(AssignExpr* expr)
{
    expr->value->accept(this);
    std::string value = last_temp;
    
    emitAssignment(expr->name, value);
    last_temp = expr->name; // El resultado de la asignación es el valor asignado
}

void CILGenerator::visit(IfExpr* expr)
{
    expr->condition->accept(this);
    std::string condition = last_temp;
    
    std::string else_label = newLabel();
    std::string end_label = newLabel();
    std::string result_temp = newTemp();
    
    // Si la condición es falsa, saltar al else
    emitInstruction("IF " + condition + " == 0 GOTO " + else_label);
    
    // Generar rama then
    expr->thenBranch->accept(this);
    emitAssignment(result_temp, last_temp);
    emitInstruction("GOTO " + end_label);
    
    // Generar rama else
    code_section << else_label << ":\n";
    expr->elseBranch->accept(this);
    emitAssignment(result_temp, last_temp);
    
    code_section << end_label << ":\n";
    
    last_temp = result_temp;
}

void CILGenerator::visit(WhileExpr* expr)
{
    std::string loop_start = newLabel();
    std::string loop_end = newLabel();
    
    code_section << loop_start << ":\n";
    
    // Evaluar condición
    expr->condition->accept(this);
    std::string condition = last_temp;
    
    // Si la condición es falsa, salir del bucle
    emitInstruction("IF " + condition + " == 0 GOTO " + loop_end);
    
    // Cuerpo del bucle
    expr->body->accept(this);
    
    // Volver al inicio
    emitInstruction("GOTO " + loop_start);
    
    code_section << loop_end << ":\n";
}

void CILGenerator::visit(NewExpr* expr)
{
    // Evaluar argumentos del constructor
    std::vector<std::string> args;
    for (auto& arg : expr->args)
    {
        arg->accept(this);
        args.push_back(last_temp);
    }
    
    last_temp = newTemp();
    
    // Crear nueva instancia según especificación
    emitInstruction(last_temp + " = ALLOCATE " + expr->typeName);
    
    // Inicializar atributos con valores por defecto si hay argumentos
    if (!args.empty())
    {
        // Llamar al constructor usando VCALL
        for (const auto& arg : args)
        {
            emitInstruction("PARAM " + arg);
        }
        emitInstruction("PARAM " + last_temp);  // self como primer parámetro
        std::string result_temp = newTemp();
        emitInstruction(result_temp + " = VCALL " + expr->typeName + " init");
    }
}

void CILGenerator::visit(SelfExpr* expr)
{
    last_temp = "self";
}

void CILGenerator::visit(BaseExpr* expr)
{
    last_temp = "base";
}

void CILGenerator::visit(MemberAccessExpr* expr)
{
    expr->object->accept(this);
    std::string object = last_temp;
    
    last_temp = newTemp();
    emitInstruction(last_temp + " = GETATTR " + object + " " + expr->member);
}

void CILGenerator::visit(MemberAssignExpr* expr)
{
    expr->object->accept(this);
    std::string object = last_temp;
    
    expr->value->accept(this);
    std::string value = last_temp;
    
    emitInstruction("SETATTR " + object + " " + expr->member + " " + value);
    last_temp = value; // El resultado de la asignación es el valor asignado
}

void CILGenerator::visit(ExprBlock* expr)
{
    // Generar código para cada statement en el bloque
    for (auto& stmt : expr->stmts)
    {
        stmt->accept(this);
    }
    // El resultado del bloque es el resultado de la última expresión
    // que ya está almacenado en last_temp
}