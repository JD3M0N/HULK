#include "cil_generator.hpp"
#include <iostream>
#include <iomanip>

CILGenerator::CILGenerator()
{
    // Inicializar secciones
    data_section << ".DATA\n";
    code_section << ".CODE\n";
}

std::string CILGenerator::generateCode(Program* program)
{
    // Generar código para el programa
    program->accept(this);
    
    // Construir el resultado final
    std::ostringstream result;
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

// ============ StmtVisitor ============

void CILGenerator::visit(Program* prog)
{
    
    // Primero generar todas las funciones
    for (auto& stmt : prog->stmts)
    {
        auto* func = dynamic_cast<FunctionDecl*>(stmt.get());
        if (func)
        {
            func->accept(this);
        }
    }
    
    // Crear función main para el código restante
    std::vector<StmtPtr> main_stmts;
    for (auto& stmt : prog->stmts)
    {
        auto* func = dynamic_cast<FunctionDecl*>(stmt.get());
        if (!func) // No es función
        {
            main_stmts.push_back(std::unique_ptr<Stmt>(stmt.release()));
        }
    }
    
    if (!main_stmts.empty())
    {
        code_section << "\nfunction main {\n";
        current_function = "main";
        
        // Procesar statements del main
        for (auto& stmt : main_stmts)
        {
            stmt->accept(this);
        }
        
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
    // Para clases, generaremos métodos como funciones separadas
    // y mantendremos información sobre atributos
    
    for (auto& method : cls->methods)
    {
        auto* func = dynamic_cast<FunctionDecl*>(method.get());
        if (func)
        {
            // Prefijo del método con el nombre de la clase
            std::string old_name = func->name;
            func->name = cls->name + "_" + func->name;
            func->accept(this);
            func->name = old_name; // Restaurar nombre original
        }
    }
}

// ============ ExprVisitor ============

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
    case BinaryExpr::OP_CONCAT: op = "@"; break; // Asumiendo @ para concatenación
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
    // Evaluar inicializador
    expr->initializer->accept(this);
    std::string init_value = last_temp;
    
    // Asignar a la variable
    emitAssignment(expr->name, init_value);
    
    // Evaluar cuerpo
    expr->body->accept(this);
    // El resultado del let es el resultado del cuerpo
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
    
    // Crear nueva instancia
    emitInstruction(last_temp + " = ALLOCATE " + expr->typeName);
    
    // Llamar al constructor si hay argumentos
    if (!args.empty())
    {
        std::string ctor_call = "CALL " + expr->typeName + "_init " + last_temp;
        for (const auto& arg : args)
        {
            ctor_call += " " + arg;
        }
        emitInstruction(ctor_call);
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