#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <omp.h>
#include <algorithm>

// -ffinite-loops -funroll-loops -faggressive-loop-optimizations -floop-nest-optimize -O1

using namespace std;

// Function to transpose a matrix
void matTranspose(float **transposed, int N)
{
    int temp;
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (transposed[i][j] != transposed[j][i])
            {
                temp = transposed[i][j];
                transposed[i][j] = transposed[j][i];
                transposed[j][i] = temp;
            }
        }
    }
}

bool checkSym(float **matrix, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (matrix[i][j] != matrix[j][i])
            {
                return false;
            }
        }
    }

    return true;
}

// Function to transpose a matrix using OpenMP
void matTransposeOMP(float** transposed, int N, int block_size, int num_thread) {
    int temp;
    // Parallelize the block-level loops
    #pragma omp parallel for collapse(2) private(temp) num_threads(num_thread)
    for (int block_i = 0; block_i < N; block_i += block_size) {
        for (int block_j = block_i; block_j < N; block_j += block_size) {
            
            // Process each block's elements
            for (int i = block_i; i < min(block_i + block_size, N); i++) {
                for (int j = block_j; j < min(block_j + block_size, N); j++) {
                    // Ensure we only transpose elements where i < j
                    if (i < j) {
                        // Swap transposed[i][j] and transposed[j][i]
                        temp = transposed[i][j];
                        transposed[i][j] = transposed[j][i];
                        transposed[j][i] = temp;
                    }
                }
            }
        }
    }
}

bool checkSymOMP(float **matrix, int N, int num_thread)
{
    bool isSymmetric = true;

#pragma omp parallel for reduction(&& : isSymmetric) num_threads(num_thread)
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (matrix[i][j] != matrix[j][i])
            {
                isSymmetric = false;
            }
        }
    }

    return isSymmetric;
}

int main()
{

    ofstream ResultFile;
    ResultFile.open("result.csv");

    int size[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    int block_size[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    for (int i = 0; i < 9; i++)
    {
        float **matrix = new float *[size[i]];
        float **transposed_serial = new float *[size[i]];
        float **transposed_parallel = new float *[size[i]];

        srand(time(NULL));

        // Inizialize the matrix
        for (int j = 0; j < size[i]; j++)
        {
            matrix[j] = new float[size[i]];
            transposed_serial[j] = new float[size[i]];
            transposed_parallel[j] = new float[size[i]];
            for (int k = 0; k < size[i]; k++)
            {
                matrix[j][k] = (float)(rand() % 100 + 1);
                transposed_serial[j][k] = matrix[j][k];
                transposed_parallel[j][k] = matrix[j][k];
            }
        }

        ResultFile << "Matrix Size" << endl
                   << size[i] << endl;

        for (int j = 0; ((block_size[j] <= size[i]) && (j < 13)); j++)
        {

            ResultFile << "Block size" << endl
                       << block_size[j] << endl;
            ResultFile << "Cores" << "," << "Serial Time" << "," << "Parallel Time" << "," << "SpeedUp" << endl;

            double start_serial = omp_get_wtime();

            if (!checkSym(matrix, size[i]))
            {
                matTranspose(transposed_serial, size[i]);
            }

            double end_serial = omp_get_wtime();
            double duration_serial = (end_serial - start_serial);

            for (int num_thread = 1; num_thread <= 4; num_thread++)
            {
                for (int k = 0; k < 10; k++)
                {
                    double start_parallel = omp_get_wtime();

                    if (!checkSymOMP(matrix, size[i], num_thread))
                    {
                        matTransposeOMP(transposed_parallel, size[i], block_size[j], num_thread);
                    }

                    double end_parallel = omp_get_wtime();
                    double duration_parallel = (end_parallel - start_parallel);

                    ResultFile << num_thread << "," << duration_serial << "," << duration_parallel << "," << duration_serial / duration_parallel << endl;
                }
            }
        }

        // Cleanup memory
        for (int j = 0; j < size[i]; j++)
        {
            delete[] matrix[j];
            delete[] transposed_serial[j];
            delete[] transposed_parallel[j];
        }
        delete[] matrix;
        delete[] transposed_serial;
        delete[] transposed_parallel;
    }

    ResultFile.close();

    return 0;
}