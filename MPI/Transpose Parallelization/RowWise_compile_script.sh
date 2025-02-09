#!/bin/bash

# Check argument
if [ $# -lt 3 ]; then
  echo "Too few arguments"
    exit 1
fi

# Compile the C++ file
mpicxx RowWise_comparison.cpp -o RowWise

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "RowWise with $2 THREADS:"
for i in $( eval echo {1..$1} ); do
    echo "Run #$i:"
    mpirun -np "$2" ./RowWise "$3"
    echo "------------------------"
done

rm RowWise