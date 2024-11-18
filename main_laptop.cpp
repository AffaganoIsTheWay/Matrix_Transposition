#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <omp.h>

// -ffinite-loops -funroll-loops -faggressive-loop-optimizations -floop-nest-optimize -O1

using namespace std;

#define MAX_ROWS 15000 // Maximum number of rows
#define MAX_COLS 15000 // Maximum number of columns

// Function to transpose a matrix
void transpose_serial(int **matrix, int **transposed)
{
    for (int i = 0; i < MAX_ROWS; ++i)
    {
        for (int j = 0; j < MAX_COLS; ++j)
        {
            transposed[j][i] = matrix[i][j];
        }
    }
}

// Function to transpose a matrix using OpenMP
void transpose_parallel(int **matrix, int **transposed, int num_thread)
{
    // Parallelize the outer loop with static schedule
#pragma omp parallel for collapse(2) schedule(guided) num_threads(num_thread)
    for (int i = 0; i < MAX_ROWS; ++i)
    {
        for (int j = 0; j < MAX_COLS; ++j)
        {
            transposed[j][i] = matrix[i][j];
        }
    }
}


int main()
{
    int **matrix = new int *[MAX_ROWS];
    int **transposed_serial = new int *[MAX_COLS];
    int **transposed_parallel = new int *[MAX_COLS];

    double start_parallel, end_parallel, duration_parallel;

    ofstream ResultFile;
    ResultFile.open("result.csv");
    ResultFile << "Cores" << "," << "Serial Time" << "," << "Parallel Time" << "," << "SpeedUp" << endl;

    srand(time(NULL));

    // Inizialize the matrix
    for (int i = 0; i < MAX_ROWS; i++)
    {
        matrix[i] = new int[MAX_COLS];
        for (int j = 0; j < MAX_COLS; j++)
        {
            matrix[i][j] = rand() % 100 + 1;
        }
    }

    // Inizialize trasposed
    for (int i = 0; i < MAX_COLS; i++)
    {
        transposed_serial[i] = new int[MAX_ROWS];
        transposed_parallel[i] = new int[MAX_ROWS];
    }

    double start_serial = omp_get_wtime();

    transpose_serial(matrix, transposed_serial);

    double end_serial = omp_get_wtime();
    double duration_serial = (end_serial - start_serial);


    for (int num_thread = 1; num_thread <= 4; num_thread++)
    {
        for (int i = 0; i < 10; i++)
        {
            start_parallel = omp_get_wtime();

            transpose_parallel(matrix, transposed_parallel, num_thread);

            end_parallel = omp_get_wtime();
            duration_parallel = (end_parallel - start_parallel);
            ResultFile << num_thread << "," << duration_serial << "," << duration_parallel << "," << duration_serial / duration_parallel << endl;
        }
    }

    // Cleanup memory
    for (int i = 0; i < MAX_ROWS; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;

    for (int i = 0; i < MAX_COLS; i++)
    {
        delete[] transposed_serial[i];
        delete[] transposed_parallel[i];
    }
    delete[] transposed_serial;
    delete[] transposed_parallel;

    ResultFile.close();

    return 0;
}