#!/bin/sh

find specs -name "*.monga" |
while read filename
do
    echo "Evaluating $filename: \c"
    ./monga "$filename" > "temp_output.out" 2> /dev/null
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo "... Valid File"
    else
        echo "... Invalid File"
    fi

    CHECK_FILE=${filename/monga/expected}

    diff "temp_output.out" $CHECK_FILE
    OUT=$?

    if [ $OUT -eq 0 ]; then
        echo "... Tests OK"
    else
        echo "... Test Error"
    fi

    echo ""
    rm temp_output.out
done
