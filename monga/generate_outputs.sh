./monga specs/basic.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/basic.spec.expected
./monga specs/condicionais.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/condicionais.spec.expected
./monga specs/main.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/main.spec.expected
./monga specs/string.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/string.spec.expected
./monga specs/loop.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/loop.spec.expected
./monga specs/blocos.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/blocos.spec.expected
./monga specs/comandos.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/comandos.spec.expected
./monga specs/type_check.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/type_check.spec.expected
./monga specs/quicksort.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/quicksort.spec.expected
./monga specs/bubble.spec.monga > monga.S && gcc -m32 -o monga_spec monga.S && ./monga_spec > specs/bubble.spec.expected
