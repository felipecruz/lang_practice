LEX=flex
YACC=yacc -d -y
FLAGS=
CC=gcc

all: lex.yy.c y.tab.h
	$(CC) lex.yy.c y.tab.c $(FLAGS) -o monga

y.tab.h:
	$(YACC) monga.y

lex.yy.c:
	$(LEX) monga.lex

clean:
	rm -f lex.yy.c y.tab.c y.tab.h monga

test:
	@./run_specs.sh
