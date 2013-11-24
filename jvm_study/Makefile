all:
	javac Teste.java && java Teste Arg1

native:
	gcc -I/System/Library/Java/JavaVirtualMachines/1.6.0.jdk/Contents/Home/include -framework JavaVM load_java_from_c.c -o load_java_from_c

bench: all native
	time ./load_java_from_c
