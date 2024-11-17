#!/bin/bash

# Compile the C++ file
g++ -o guided_transpose guided_transposition.cpp -fopenmp

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Guided"
echo "$1 THREADS:"
# Run the executable 10 times
for i in {1..10}; do
    echo "Run #$i:"
    export OMP_NUM_THREADS="$1"; ./guided_transpose
    echo "------------------------"
done

rm guided_transpose