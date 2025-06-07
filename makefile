# Detect OS and set platform‐specific commands and paths
ifeq ($(OS),Windows_NT)
  UNAME_S   := Windows
  DIRSEP    := \\
  RM        := del /Q
  RMDIR     := rmdir /S /Q
  MKDIR     := mkdir build 2>nul
  OBJ_FILES := src\main.o \
               src\lexer\HULKLexer.o \
               src\parser\HULKParser.o \
               src\AST\ast.o
  OUT_BIN   := build\script.hulk.exe
else
  UNAME_S   := $(shell uname -s)
  DIRSEP    := /
  RM        := rm -f
  RMDIR     := rm -rf
  MKDIR     := mkdir -p build
  OBJ_FILES := src/main.o \
               src/lexer/HULKLexer.o \
               src/parser/HULKParser.o \
               src/AST/ast.o
  OUT_BIN   := build/script.hulk
endif

CXX      := g++
CXXFLAGS := -std=c++17 -g -O0 -Wall -Isrc

# Default
all: compile

# Build: ensure build/ exists, then link
compile: $(OBJ_FILES)
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -o $(OUT_BIN) $(OBJ_FILES)

# Compile each .cpp into its .o
src/main.o: src/main.cpp src/lexer/HULKLexer.h src/parser/HULKParser.h token/Token.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/lexer/HULKLexer.o: src/lexer/HULKLexer.cpp src/lexer/HULKLexer.h token/Token.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/parser/HULKParser.o: src/parser/HULKParser.cpp src/parser/HULKParser.h \
                        src/lexer/HULKLexer.h token/Token.h src/AST/AST.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/AST/AST.o: src/AST/ast.cpp src/AST/ast.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
ifeq ($(UNAME_S),Windows)
	@echo Cleaning for Windows...
	@$(RM) $(OBJ_FILES) $(OUT_BIN) 2>nul
	@$(RMDIR) build
else
	@echo Cleaning for Unix...
	@$(RM) $(OBJ_FILES) $(OUT_BIN)
	@$(RMDIR) build
endif

.PHONY: all compile clean
