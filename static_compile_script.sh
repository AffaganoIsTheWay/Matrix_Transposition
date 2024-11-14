#!/bin/bash

# Compile the C++ file
g++ -o static_transpose static_transposition.cpp -fopenmp

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi


echo "2 THREADS:"
# Run the executable 10 times
for i in {1..10}; do
    echo "Run #$i:"
    export OMP_NUM_THREADS=2; ./static_transpose
    echo "------------------------"
done
echo "4 THREADS:"
# Run the executable 10 times
for i in {1..10}; do
    echo "Run #$i:"
    export OMP_NUM_THREADS=4; ./static_transpose
    echo "------------------------"
done

rm static_transpose