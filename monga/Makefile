LEX=flex
YACC=yacc -d -y -v
FLAGS=-g
CC=gcc

all: clean lex.yy.c y.tab.h
	@$(CC) astnodes.c codegen.c typesystem.c dump.c lex.yy.c y.tab.c $(FLAGS) -o monga

debug: clean lex.yy.c y.tab.h
	@$(CC) astnodes.c codegen.c typesystem.c dump.c lex.yy.c y.tab.c $(FLAGS) -o monga -DDEBUG=1

y.tab.h:
	@$(YACC) monga.y

lex.yy.c:
	@$(LEX) monga.lex

clean:
	@rm -f lex.yy.c
	@rm -f y.tab.c
	@rm -f y.tab.h
	@rm -f monga
	@rm -rf dump

dump: clean
	@gcc astnodes.c dump.c -Wall -o dump -DDUMP_TEST=1
	@./dump

test: all
	@./run_specs.sh
