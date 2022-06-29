all: parser

CC := g++

# Lexer and parser
LEX := flex
YACC := bison
SRC_CPP := main.cpp

# file
LEX_FILE := lex.l
YACC_FILE := parser.y

LEX_CPP := $(LEX_FILE:.l=.cpp)
LEX_HPP := $(LEX_FILE:.l=.hpp)
YACC_CPP := $(YACC_FILE:.y=.cpp)
YACC_HPP := $(YACC_FILE:.y=.hpp)
YACC_C := $(YACC_FILE:.y=.tab.c)
YACC_H := $(YACC_FILE:.y=.tab.h)
YACC_OUTPUT := $(YACC_FILE:.y=.output)

OBJ := $(LEX_CPP:.cpp=.o) $(YACC_CPP:.cpp=.o) $(SRC_CPP:.cpp=.o)
BIN := c2ir

LLVMCONFIG := llvm-config
CPPFLAGS := `$(LLVMCONFIG) --cppflags` -std=c++14
LDFLAGS := `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

parser: $(LEX_CPP) $(YACC_CPP) $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS) $(LIBS)

$(LEX_CPP): $(LEX_FILE)
	$(LEX) --header-file=$*.hpp -o $*.cpp $<

$(YACC_CPP): $(YACC_FILE)
	$(YACC) -d -v -o $*.cpp $<

%.o : %.cpp
	$(CC) -c $< $(CPPFLAGS)

test: all
	cat text.c | ./$(BIN)

clean:
	rm -f $(LEX_CPP) $(YACC_CPP) $(LEX_HPP) $(YACC_HPP)
	rm -f $(YACC_C) $(YACC_H) $(YACC_OUTPUT)
	rm -f $(OBJ)
	rm -f $(BIN)
