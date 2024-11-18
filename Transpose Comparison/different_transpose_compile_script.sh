#!/bin/bash

# Check argument
if [ $# -lt 2 ]; then
  echo "Too few arguments"
    exit 1
fi

# Compile the C++ file
g++ -o different_transposition different_matrix_transposition.cpp -fopenmp

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Different Transposition:"
# Run the executable 10 times
for i in $( eval echo {1..$1} ); do
    echo "Run #$i:"
    ./different_transposition "$2"
    echo "------------------------"
done

rm different_transposition