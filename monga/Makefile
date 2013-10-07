LEX=flex
YACC=yacc -d -y -v
FLAGS=
CC=gcc

all: clean lex.yy.c y.tab.h
	$(CC) astnodes.c lex.yy.c y.tab.c $(FLAGS) -o monga

y.tab.h:
	$(YACC) monga.y

lex.yy.c:
	$(LEX) monga.lex

clean:
	@rm -f lex.yy.c
	@rm -f y.tab.c
	@rm -f y.tab.h
	@rm -f monga
	@rm -rf dump

dump: clean
	@gcc dump.c -Wall -o dump -DDUMP_TEST=1
	@./dump

test:
	@./run_specs.sh
