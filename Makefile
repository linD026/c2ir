PWD := $(CURDIR)

# C config
INC=$(PWD)
INC_PARAMS=$(INC:%=-I%)

CC := gcc

# Lexer and parser
LEX := flex
YACC := bison

# file
LEX_FILE := lex.l
YACC_FILE := parser.y

GENERATED_C_LEX := $(LEX_FILE:.l=.c)
GENERATED_H_LEX := $(LEX_FILE:.l=.h)

GENERATED_C_YACC := $(YACC_FILE:.y=.c)
GENERATED_H_YACC := $(YACC_FILE:.y=.h)

GENERATED_C_FILE := $(GENERATED_C_YACC) $(GENERATED_C_LEX)

GENERATED_FILE := $(GENERATED_H_YACC) $(GENERATED_H_LEX)
GENERATED_FILE := $(GENERATED_H_YACC) $(GENERATED_H_LEX)
GENERATED_FILE += $(GENERATED_C_FILE) $(GENERATED_C_FILE:.c=.o)
GENERATED_FILE += $(YACC_FILE:.y=.output)

OBJ := $(GENERATED_C_FILE:.c=.o)
BIN := test

all: $(GENERATED_C_LEX) $(GENERATED_C_YACC) $(OBJ)
	$(CC) $(CFLAGS) $(INC_PARAMS) $(OBJ) -o $(BIN)

$(GENERATED_C_LEX): $(LEX_FILE)
	$(LEX) --header-file=$*.h -o $*.c $<

$(GENERATED_C_YACC): $(YACC_FILE)
	$(YACC) -d -v -o $*.c $<

%.o : %.c
	$(CC) -c $<

clean:
	rm -f $(GENERATED_FILE)
	rm -f $(BIN)
