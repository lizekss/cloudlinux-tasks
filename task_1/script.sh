#!/bin/bash

# Find all .c files in the working directory and its subdirectories
c_files=$(find . -type f -name "*.c")

# Iterate over each found .c file
for c_file in $c_files; do
    # Copy each file to a new file with .orig extension added
    cp "$c_file" "${c_file}.orig"
done