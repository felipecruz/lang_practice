LEX=flex
YACC=yacc -yd
CC=gcc

all: y.tab.o lex.yy.o
	$(CC) -o monga y.tab.o lex.yy.o

y.tab.o: lex.yy.c y.tab.h
	$(CC) y.tab.c -o y.tab.o

lex.yy.o: lex.yy.c y.tab.h
	$(CC) lex.yy.c -o lex.yy.o

y.tab.h:
	$(YACC) monga.y

lex.yy.c:
	$(LEX) monga.lex

clean:
	rm -f lex.yy.o lex.yy.c y.tab.o y.tab.c y.tab.h monga

test:
	@./run_specs.sh
