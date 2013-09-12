all:
	flex monga.lex
	gcc lex.yy.c -lfl -o monga

test:
	@./run_specs.sh

gen_output:
	-@./generate_outputs.sh > /dev/null 2>&1
