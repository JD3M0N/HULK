
#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <memory>
#include <sstream>
#include <cctype>

using namespace std;

// ————————————————
// 1. Definición de símbolos
// ————————————————
enum Symbol
{
    END,    // $
    NUMBER, // literal numérico
    PLUS,   // +
    MINUS,  // -
    STAR,   // *
    SLASH,  // /
    LPAREN, // (
    RPAREN, // )
    E,      // Expresión / E'
    T,      // Término / T'
    F       // Factor
};

struct ParseNode
{
    Symbol type;
    string value;
    vector<shared_ptr<ParseNode>> children;

    ParseNode(Symbol t, string v = "") : type(t), value(v) {}
};
// ——————————————————————————————
// 2. Producciones corregidas (ε = vector vacío)
// ——————————————————————————————
static const vector<pair<Symbol, vector<Symbol>>> productions = {
    {E, {T, E}},              // 0: E → T E'
    {E, {}},                   // 1: E' → ε
    {E, {PLUS, T, E}},        // 2: E' → + T E'
    {E, {MINUS, T, E}},       // 3: E' → - T E'
    {T, {F, T}},              // 4: T → F T'
    {T, {}},                   // 5: T' → ε
    {T, {STAR, F, T}},        // 6: T' → * F T'
    {T, {SLASH, F, T}},       // 7: T' → / F T'
    {F, {LPAREN, E, RPAREN}}, // 8: F → ( E )
    {F, {NUMBER}},            // 9: F → number
    {F, {MINUS, F}}           // 10: F → - F
};

// Tabla ACTION [estado][símbolo] -> acción
static const map<int, map<Symbol, string>> action_table = {
    {0,  {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {E, "1"}, {T, "2"}, {F, "3"}}},
    {1,  {{PLUS, "s7"}, {MINUS, "s8"}, {END, "acc"}}},
    {2,  {{PLUS, "r1"}, {MINUS, "r1"}, {STAR, "s9"}, {SLASH, "s10"}, {RPAREN, "r1"}, {END, "r1"}}},
    {3,  {{PLUS, "r5"}, {MINUS, "r5"}, {STAR, "r5"}, {SLASH, "r5"}, {RPAREN, "r5"}, {END, "r5"}}},
    {4,  {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {E, "11"}, {T, "2"}, {F, "3"}}},
    {5,  {{PLUS, "r9"}, {MINUS, "r9"}, {STAR, "r9"}, {SLASH, "r9"}, {RPAREN, "r9"}, {END, "r9"}}},
    {6,  {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {F, "12"}}},
    {7,  {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {T, "13"}, {F, "3"}}},
    {8,  {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {T, "14"}, {F, "3"}}},
    {9,  {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {F, "15"}}},
    {10, {{NUMBER, "s5"}, {MINUS, "s6"}, {LPAREN, "s4"}, {F, "16"}}},
    {11, {{PLUS, "s7"}, {MINUS, "s8"}, {RPAREN, "s17"}}},
    {12, {{PLUS, "r10"}, {MINUS, "r10"}, {STAR, "r10"}, {SLASH, "r10"}, {RPAREN, "r10"}, {END, "r10"}}},
    {13, {{PLUS, "r2"}, {MINUS, "r2"}, {STAR, "s9"}, {SLASH, "s10"}, {RPAREN, "r2"}, {END, "r2"}}}, // Fix: r2 para PLUS
    {14, {{PLUS, "r3"}, {MINUS, "r3"}, {STAR, "s9"}, {SLASH, "s10"}, {RPAREN, "r3"}, {END, "r3"}}}, // Fix: r3 para PLUS
    {15, {{PLUS, "r6"}, {MINUS, "r6"}, {STAR, "r6"}, {SLASH, "r6"}, {RPAREN, "r6"}, {END, "r6"}}},
    {16, {{PLUS, "r7"}, {MINUS, "r7"}, {STAR, "r7"}, {SLASH, "r7"}, {RPAREN, "r7"}, {END, "r7"}}},
    {17, {{PLUS, "r8"}, {MINUS, "r8"}, {STAR, "r8"}, {SLASH, "r8"}, {RPAREN, "r8"}, {END, "r8"}}}
};

// Tabla GOTO [estado][no-terminal] -> estado
static const map<int, map<Symbol, int>> goto_table = {
    {0,  {{E, 1}, {T, 2}, {F, 3}}},
    {2,  {{E, -1}, {T, -1}, {F, -1}}}, // Fix: Evitar GOTO inválido
    {4,  {{E, 11}, {T, 2}, {F, 3}}},
    {6,  {{F, 12}}},
    {7,  {{T, 13}, {F, 3}}},
    {8,  {{T, 14}, {F, 3}}},
    {9,  {{F, 15}}},
    {10, {{F, 16}}},
    {13, {{E, -1}, {T, -1}, {F, -1}}}, // Fix: Estados finales
    {14, {{E, -1}, {T, -1}, {F, -1}}}
};


// ——————————————————————————————————————————————
// 4. Tokenizador mejorado (ss.get + isspace para separar números)
// ——————————————————————————————————————————————
vector<pair<Symbol, string>> tokenize(const string &expr)
{
    vector<pair<Symbol, string>> tokens;
    stringstream ss(expr);
    char c;
    string num_buf;

    auto flush_number = [&]()
    {
        if (!num_buf.empty())
        {
            tokens.emplace_back(NUMBER, num_buf);
            num_buf.clear();
        }
    };

    while (ss.get(c))
    {
        if (isdigit(c) || c == '.')
        {
            num_buf += c;
        }
        else
        {
            // antes de procesar operador, vuelca número acumulado
            flush_number();
            if (isspace(static_cast<unsigned char>(c)))
                continue;
            switch (c)
            {
            case '+':
                tokens.emplace_back(PLUS, "+");
                break;
            case '-':
                tokens.emplace_back(MINUS, "-");
                break;
            case '*':
                tokens.emplace_back(STAR, "*");
                break;
            case '/':
                tokens.emplace_back(SLASH, "/");
                break;
            case '(':
                tokens.emplace_back(LPAREN, "(");
                break;
            case ')':
                tokens.emplace_back(RPAREN, ")");
                break;
            default:
                throw runtime_error(string("Carácter inválido: ") + c);
            }
        }
    }
    flush_number();
    tokens.emplace_back(END, "$");
    return tokens;
}

// ——————————————————————————————————————————————
// 5. Parser LR(0) bottom‑up
// ——————————————————————————————————————————————
class LRParser
{
    stack<int> state_stack;
    stack<shared_ptr<ParseNode>> parse_stack;
    vector<pair<Symbol, string>> input;
    size_t pos = 0;

public:
    LRParser(const string &expr)
    {
        input = tokenize(expr);
        state_stack.push(0);
    }

    shared_ptr<ParseNode> parse()
    {
        while (true)
        {
            int st = state_stack.top();
            Symbol look = input[pos].first;

            auto it = action_table.at(st).find(look);
            if (it == action_table.at(st).end())
                throw runtime_error("Error de sintaxis en token #" + to_string(pos));

            const string &act = it->second;
            if (act[0] == 's')
            {
                // — Shift —
                int ns = stoi(act.substr(1));
                state_stack.push(ns);
                parse_stack.push(make_shared<ParseNode>(look, input[pos].second));
                pos++;
            }
            else if (act[0] == 'r')
            {
                // — Reduce —
                int pid = stoi(act.substr(1));
                const auto &prod = productions[pid];
                int k = prod.second.size();
                auto node = make_shared<ParseNode>(prod.first);

                // si k>0 pop x k y
                for (int i = 0; i < k; ++i)
                {
                    state_stack.pop();
                    node->children.insert(node->children.begin(), parse_stack.top());
                    parse_stack.pop();
                }
                // goto
                int top = state_stack.top();
                int ns = goto_table.at(top).at(prod.first);
                state_stack.push(ns);
                parse_stack.push(node);
            }
            else
            { // acc
                return parse_stack.top();
            }
        }
    }
};

// ——————————————————————————————————————————————
// 6. Impresión legible del árbol
// ——————————————————————————————————————————————
void print_tree(const shared_ptr<ParseNode> &n, int depth = 0)
{
    string indent(depth * 2, ' ');
    string sym;
    switch (n->type)
    {
    case E:
        sym = "E";
        break;
    case T:
        sym = "T";
        break;
    case F:
        sym = "F";
        break;
    case PLUS:
        sym = "+";
        break;
    case MINUS:
        sym = "-";
        break;
    case STAR:
        sym = "*";
        break;
    case SLASH:
        sym = "/";
        break;
    case LPAREN:
        sym = "(";
        break;
    case RPAREN:
        sym = ")";
        break;
    case NUMBER:
        sym = n->value;
        break;
    case END:
        sym = "$";
        break;
    default:
        sym = "?";
        break;
    }
    cout << indent << sym << "\n";
    for (auto &ch : n->children)
        print_tree(ch, depth + 1);
}

// ——————————————————————————————————————————————
// 7. Ejemplo de uso
// ——————————————————————————————————————————————
int main()
{
    cout << "Ingrese una expresión: ";
    string line;
    getline(cin, line);

    try
    {
        LRParser parser(line);
        auto tree = parser.parse();
        cout << "\nÁrbol de análisis:\n";
        print_tree(tree);
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
