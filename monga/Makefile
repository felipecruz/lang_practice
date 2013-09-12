all:
	flex monga.lex
	gcc lex.yy.c -lfl -o monga

test:
	@./run_specs.sh
