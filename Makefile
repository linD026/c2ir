PWD := $(CURDIR)

# C config
INC=$(PWD)
INC_PARAMS=$(INC:%=-I%)

CC := gcc
CFLAGS+=-Wall
CFLAGS+=-O1

# Lexer and parser
LEXER := flex
PARSER := bison

lexer.h lexer.c: lexer.l
	$(LEXER) --header-file=lexer.h -o lexer.c lexer.l

parser.h parser.c: parser.y
	$(PARSE) -d -v -o parser.c parser.y

%.o: %.c
	$(CC) $(CFLAGS) $(INC_PARAMS) -c $< -o $@

c-files: parser.c lexer.c

all: c-files
