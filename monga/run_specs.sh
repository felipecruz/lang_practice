#!/bin/bash

FILES=specs/*

for f in $FILES
do
  echo "Processing $f file..."
  ./monga < $f
done
