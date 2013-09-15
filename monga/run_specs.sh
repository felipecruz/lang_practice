#!/bin/bash

find specs -name "*.monga" |
while read filename
do
    echo "Evaluating $filename: \c"
    ./monga "$filename" > "temp_output.out" 2> /dev/null
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo "... Valid Syntax\c"
    else
        echo "... Invalid Syntax\c"
    fi

    CHECK_FILE=${filename/monga/expected}

    diff "temp_output.out" $CHECK_FILE
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo "... Tests OK"
    else
        echo "... Test Error"
    fi

    rm temp_output.out
done
