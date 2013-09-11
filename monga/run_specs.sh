#!/bin/sh

find specs -name "*.monga" |
while read filename
do
    echo "Evaluating $filename: \c"
    ./monga "$filename" > /dev/null 2>&1
    OUT=$?
    if [ $OUT -eq 0 ]; then
        echo "... Valid File"
    else
        echo "... Invalid File"
    fi
done
