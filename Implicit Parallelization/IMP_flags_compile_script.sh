#!/bin/bash

# Check argument
if [ $# -lt 2 ]; then
  echo "Too few arguments"
    exit 1
fi

# Optimization flags to test
OPT_FLAGS=(
    "-O1"
    "-funroll-loops"
    "-floop-unroll-and-jam"
    "-fprefetch-loop-arrays "
    "-ftree-vectorize"
    "-funroll-loops -O1"
    "-floop-unroll-and-jam -O1"
    "-fprefetch-loop-arrays -O1"
    "-ftree-vectorize -O1"
)

# Loop over each optimization flag
for OPT_FLAG in "${OPT_FLAGS[@]}"; do

    # Compile the C++ file
    g++ -o imp_flags IMP_flags.cpp -fopenmp $OPT_FLAG
    
    # Check if compilation was successful
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed with flag $OPT_FLAG"
        continue
    fi
    
    # Run the compiled program and measure execution time
    echo "Runnig this flags: $OPT_FLAG"
    # Run the executable 10 times
    for i in $( eval echo {1..$1} ); do
        echo "Run #$i:"
        ./imp_flags "$2"
        echo "------------------------"
    done
done

rm imp_flags