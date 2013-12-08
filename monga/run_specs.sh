#!/bin/bash

find specs -name "*.monga" |
while read filename
do
    echo -n "Evaluating $filename: "
    ./monga "$filename" > monga.S && gcc -m32 -o monga_spec monga.S 2> /dev/null
    ./monga_spec > "temp_output.out" 2> /dev/null
    
    CHECK_FILE=${filename/monga/expected}

    diff "temp_output.out" $CHECK_FILE
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo "... Tests OK"
    else
        echo "ERROR"
        exit
    fi

    rm temp_output.out
done
