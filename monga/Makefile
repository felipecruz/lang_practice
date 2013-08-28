all:
	flex monga.lex
	gcc lex.yy.c -lfl -o monga

run:
	./monga < example.monga
