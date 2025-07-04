# Compilador a utilizar
CXX        = g++
CXXFLAGS   = -std=c++17 -Wall -I src

# Flex / Bison
LEXER_SRC      = src/Lexer/lexer.l
PARSER_SRC     = src/Parser/parser.y
LEXER_GEN      = src/Lexer/lex.yy.cpp
PARSER_GEN_CPP = src/Parser/parser.tab.cpp
PARSER_GEN_HPP = src/Parser/parser.tab.hpp

# Fuentes propias
MAIN_SRC         = src/main.cpp
TYPE_SRC         = src/Type/type.cpp
TYPEINF_SRC      = src/Type/type_inferer.cpp
CODEGEN_SRC      = src/CodeGen/cil_generator.cpp
MIPSGEN_SRC      = src/CodeGen/mips_generator.cpp
ERROR_SRC        = src/Error/error_handler.cpp

# Objetos resultantes
OBJS = \
	$(PARSER_GEN_CPP:.cpp=.o) \
	$(LEXER_GEN:.cpp=.o)    \
	$(MAIN_SRC:.cpp=.o)     \
	$(TYPE_SRC:.cpp=.o)     \
	$(TYPEINF_SRC:.cpp=.o)  \
	$(CODEGEN_SRC:.cpp=.o)  \
	$(MIPSGEN_SRC:.cpp=.o)  \
	$(ERROR_SRC:.cpp=.o)

# Ejecutable del compilador HULK
BIN_DIR    = hulk
EXECUTABLE = $(BIN_DIR)/hulk_executable

# Ruta a MARS (jar dentro de la carpeta MARS-main en la raíz)
MARS_JAR   = MARS-main/Mars.jar

.PHONY: all execute clean distclean

# Target por defecto
all: $(EXECUTABLE)

# 1) Generar parser (Bison)
$(PARSER_GEN_CPP) $(PARSER_GEN_HPP): $(PARSER_SRC)
	bison -d -o $(PARSER_GEN_CPP) $(PARSER_SRC)

# 2) Generar lexer (Flex)
$(LEXER_GEN): $(LEXER_SRC) $(PARSER_GEN_HPP)
	flex -o $(LEXER_GEN) $(LEXER_SRC)

# 3) Compilar .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4) Linkear todos los objetos
$(EXECUTABLE): $(PARSER_GEN_CPP) $(LEXER_GEN) $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# Crear carpeta de destino si hace falta
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Ejecutar: genera script.s desde script.hulk y lo lanza con MARS
# @echo "→ Generando MIPS assembly (script.s)"
execute: all
	@./$(EXECUTABLE) script.hulk
	@echo " Ejecutando script.s con MARS"
	@java -jar "$(MARS_JAR)" script.s

# Limpieza de objetos y generados
clean:
	rm -f $(OBJS) $(LEXER_GEN) $(PARSER_GEN_CPP) $(PARSER_GEN_HPP) $(EXECUTABLE) script.s

# Limpieza total (incluye carpeta de binarios)
distclean: clean
	rm -rf $(BIN_DIR)
