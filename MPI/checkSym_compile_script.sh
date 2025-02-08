#!/bin/bash

# Check argument
if [ $# -lt 3 ]; then
  echo "Too few arguments"
    exit 1
fi

# Compile the C++ file
mpicxx checkSym_comparison.cpp -o checkSym_comparison

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

echo "Comapare different checkSym with $2 THREADS:"
# Run the executable 10 times
for i in $( eval echo {1..$1} ); do
    echo "Run #$i:"
    mpirun -np "$2" ./checkSym_comparison "$3"
    echo "------------------------"
done

rm checkSym_comparison