#!/bin/bash

find specs -name "*.monga" |
while read filename
do
    echo -n "Evaluating $filename: "
    ./monga "$filename" > "temp_output.out" 2> /dev/null
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo -n "... Valid Syntax"
    else
        echo -n "... Invalid Syntax"
    fi

    CHECK_FILE=${filename/monga/expected}

    diff "temp_output.out" $CHECK_FILE
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo "... Tests OK"
    else
        echo "... Test Error"
        exit
    fi

    rm temp_output.out
done
