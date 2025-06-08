# Compilador a utilizar
CXX = g++

# Flags de compilación: -std=c++17 para compatibilidad moderna, -Wall para mostrar advertencias
CXXFLAGS = -std=c++17 -Wall -I src

# Archivo fuente del lexer (Flex)
LEXER_SRC = src/Lexer/lexer.l

# Archivo fuente del parser
PARSER_SRC = src/Parser/parser.y

# Archivo generado por Flex (lo compilaremos)
LEXER_GEN = src/Lexer/lex.yy.cpp

# Archivo generado por Bison
PARSER_GEN_CPP = src/Parser/parser.tab.cpp
PARSER_GEN_HPP = src/Parser/parser.tab.hpp

# Archivo fuente principal
MAIN_SRC = src/main.cpp

# Objetos resultantes de la compilación
OBJS = $(PARSER_GEN_CPP:.cpp=.o) $(LEXER_GEN:.cpp=.o)  $(MAIN_SRC:.cpp=.o)

# Carpeta donde se generará el ejecutable
BIN_DIR = hulk

# Nombre del ejecutable final
EXECUTABLE = $(BIN_DIR)/hulk_executable

# Target principal (default)
all: compile

# Compilación completa
compile: $(EXECUTABLE)

# Primero generar parser con Bison (genera .cpp y .hpp)
$(PARSER_GEN_CPP) $(PARSER_GEN_HPP): $(PARSER_SRC)
	bison -d -o $(PARSER_GEN_CPP) $(PARSER_SRC)

# Luego generar lexer con Flex (después de tener parser.tab.hpp)
$(LEXER_GEN): $(LEXER_SRC) $(PARSER_GEN_HPP)
	flex -o $(LEXER_GEN) $(LEXER_SRC)

# Compilación de cualquier archivo .cpp en su .o correspondiente
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compila el ejecutable a partir de los objetos
$(EXECUTABLE): $(PARSER_GEN_CPP) $(LEXER_GEN) $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# Crear carpeta 'hulk' si no existe
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Ejecutar el programa, pasando script.hulk como argumento
execute: compile
	./$(EXECUTABLE) script.hulk

# Limpiar todos los archivos generados
clean:
	rm -f $(OBJS) $(LEXER_GEN)  $(PARSER_GEN_CPP) $(PARSER_GEN_HPP) $(EXECUTABLE)

# Limpiar todo completamente (incluye la carpeta de binarios)
distclean: clean
	rm -rf $(BIN_DIR)

# xD