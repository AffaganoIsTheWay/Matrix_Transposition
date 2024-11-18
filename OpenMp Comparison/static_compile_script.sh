#!/bin/bash

# Check argument
if [ $# -lt 3 ]; then
  echo "Too few arguments"
    exit 1
fi

# Compile the C++ file
g++ -o static_transpose static_transposition.cpp -fopenmp

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Static with $2 THREADS:"
# Run the executable 10 times
for i in $( eval echo {1..$1} ); do
    echo "Run #$i:"
    export OMP_NUM_THREADS="$2"; ./static_transpose "$3"
    echo "------------------------"
done

rm static_transpose