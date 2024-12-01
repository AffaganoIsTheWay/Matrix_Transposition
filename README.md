# IntroPARCO 2024 - MATRIX TRANSPOSE

## Introduction

Matrix transpose is a simple operation where the rows of a matrix are turned into columns. It is commonly used in scientific computing, machine learning, computer graphics, and numerical simulations. In this project, we evaluate different approaches to perform matrix transposition, focusing on their computational efficiency in various environments. Specifically, we compare sequential, implicitly parallelized, and OpenMP-parallelized implementations.

To start clone the repository:

```bash
git clone https://github.com/AffaganoIsTheWay/Matrix_Transposition.git
```

## Architecture

The tests in this repository were executed on the following machine architectures:

#### Personal Computers

- **Processor**: AMD Ryzen 5 5600G 3,90 GHz (6 cores, 12 threads)
- **Architecture**: Zen 3 (x86_64)
- **RAM**: 16 GB DDR4

<br>

- **Processor**: Intel(R) Core(R) i7-7500U CPU @ 2.70GHz (4 cores, 8 threads)
- **Architecture**: x86_64
- **RAM**: 16 GB DDR4

#### Cluster

- **Processor**: Intel(R) Xeon(R) Gold 6252N CPU @ 2.30GHz (96 cores, 96 threads)
- **Architecture**: x86_64
- **RAM**: 8 GB

## Transpose Comparison

This section compares the performance of two different transpose implementations in terms of execution time and memory usage.

The following are key metrics for comparison:

- **Execution Time**: Time taken to complete the transpose of a matrix of varying sizes.
- **Memory Bandwidth**: The efficiency of memory usage during the transpose operation.

In order to run this test, first navigate to the directory:

```bash
cd Transpose\ Comparison/
```

Then run the bash script:

```bash
sh different_transpose_compile_script.sh <number of iteration> <matrix size>
```

- **number of itaration** is how many times the test will be performed.
- **matrix size** is the size of one side of the matrix (if 10 the matrix will be 10x10).

Here a possible call:

```bash
sh different_transpose_compile_script.sh 10 15000
```

## Sequential Implementation

In the sequential implementation, the matrix transpose is performed by a single thread in a straightforward manner. Each element of the matrix is moved to its transposed position one at a time.

In this section, the **Execution Time** and **Memory Bandwidth** will be measured to check if the matrix is symmetric and to compute the transpose in case is not.

To execute this measurament, first navigate to the directory:

```bash
cd Sequential\ Implementation/
```

Then run the bash script:

```bash
sh sequential_transpose_compile_script.sh <number of iteration> <matrix size>
```

- **number of itaration** is how many times the test will be performed.
- **matrix size** is the size of one side of the matrix (if 10 the matrix will be 10x10).

Here a possible call:

```bash
sh sequential_transpose_compile_script.sh 10 15000
```

## Implicit Parallelization

Implicit parallelization refers to the parallelism automatically provided by the compiler or runtime environment. Modern compilers, such as GCC, can automatically parallelize loops if certain conditions are met, depending on the hardware and architecture being used.

In this part, we evaluate **Execution Time** and **Memory Bandwidth**, first for different optimization flags, such as ```-O1``` or ```-funroll-loops```, and second for explicit unrolling in both the symmetry check and matrix transpose.

To execute this measurament, first navigate to the directory:

```bash
cd Implicit\ Parallelization/
```

To run the flags evaluation, run the bash script:

```bash
sh IMP_flags_compile_script.sh <number of iteration> <matrix size>
```

To run the explixit unroll evaluation, run the bash script:

```bash
sh explicit_unroll_compile_script.sh <number of iteration> <matrix size>
```

- **number of itaration** is how many times the test will be performed (For the flag test is number of iteration per flag).
- **matrix size** is the size of one side of the matrix (if 10 the matrix will be 10x10).

Here a possible call:

```bash
sh explicit_unroll_compile_script.sh 10 15000
```

or

```bash
sh IMP_flags_compile_script.sh 10 15000
```

## OpenMP Comparison

OpenMP is a popular API for parallel programming in C, C++, and Fortran. It enables easy parallelization of loops and sections of code by adding simple compiler directives.

In this section different OpenMP implementation are compared in order to find the best solution.

Firstly, different symmetry check implementation are compared.

Navigate to the directory:

```bash
cd OpenMp\ Comparison/
```

Then to run the check symmetry funtion comparison, run the bast script:

```bash
sh checkSym_compile_script.sh <number of iteration> <number of threads> <matrix size>
```

- **number of itaration** is how many times the test will be performed.
- **number of threads** is with how many threads you want to run the simulation with.
- **matrix size** is the size of one side of the matrix (if 10 the matrix will be 10x10).

Here a possible call

```bash
sh checkSym_compile_script.sh 10 4 15000
```

Once we had found the best implementation for the symmetry check function, we can implement it to find the best matrix transpose implementation.

Navigate to the directory:

```bash
cd Transpose\ Parallelization/
```

Here we can run each of the implementation on its own,

```bash
sh Implementation_script.sh <number of iteration> <number of threads> <matrix size>
```

Or we can all of them together with:

```bash
sh all_compile_script.sh <number of iteration> <number of threads> <matrix size>
```

Here a possible call:

```bash
sh all_compile_script.sh 10 4 15000
```

## Cluster Run

The ```main.cpp``` file is a program that will performe a matrix transpose for matrix of size from 2<sup>4</sup> to 2<sup>12</sup>, with different block size and for 1 up to 64 threads(```main_laptop.cpp``` in only up to 4 threads, so it can be runned also on a pc).

In order to run it, first, copy both ```main.cpp``` and ```main_script.pbs``` in the cluster. Open ```main_script.pbs``` and modify the working directory in to match yours. Then submit the job:

```bash
qsub main_script.pbs
```

A file named ```result.csv``` will be created, in which all the result will be saved.

### ATTENTION

```main_script.pbs``` may require to be converted to unix format.

In order to do so, run the following command on the cluster:

```bash
dos2unix main_script.pbs
```
