#include <iostream>
#include <mpi.h>
#include <ctime>
#include <cstdlib>

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

bool check_transpose(float **transposed_serial, float **transposed_parallel, int N)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (transposed_serial[i][j] != transposed_parallel[i][j])
            {
                return false;
            }
        }
    }

    return true;
}

// Function to transpose a matrix using MPI
void matTransposeMPI(float **transposed, int N, int size, int rank)
{
    // Flatten the matrix for MPI operations
    float *matrix_flat = (float *)malloc(N * N * sizeof(float));
    float *local_matrix = (float *)malloc(N * N * sizeof(float));

    // If rank 0, flatten the 2D matrix into a 1D array
    if (rank == 0)
    {
        // Copy the 2D matrix to the 1D array for scattering
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                matrix_flat[i * N + j] = transposed[i][j];
            }
        }
    }

    // Transpose the local portion of the matrix
    int temp;
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (local_matrix[i * N + j] != local_matrix[j * N + i])
            {
                temp = local_matrix[i * N + j];
                local_matrix[i * N + j] = local_matrix[j * N + i];
                local_matrix[j * N + i] = temp;
            }
        }
    }

    // If rank 0, copy back the data from the flat array to the 2D matrix
    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                transposed[i][j] = matrix_flat[i * N + j];
            }
        }
    }

    // Free the allocated memory
    free(matrix_flat);
    free(local_matrix);
}

bool checkSym_MPI(float **matrix, int N, int size, int rank)
{
    bool local_result = true;
    int block_size = N / size;
    int start_row = (rank / (N / block_size)) * block_size;
    int end_row = start_row + block_size;
    int start_col = (rank % (N / block_size)) * block_size;
    int end_col = start_col + block_size;

    // Each process checks its block of the matrix
    for (int i = start_row; i < end_row; i++)
    {
        for (int j = start_col; j < end_col; j++)
        {
            if (matrix[i][j] != matrix[j][i])
            {
                local_result = false;
                break; // No need to check further
            }
        }
        if (!local_result)
            break; // No need to check further rows if we already found an inconsistency
    }

    // Use MPI to gather results from all processes
    bool global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

    // The root process (rank 0) determines if the matrix is symmetric
    return global_result;
}

int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);
    float **matrix = new float *[N];
    float **transposed_serial = new float *[N];
    float **transposed_parallel = new float *[N];

    srand(time(NULL));

    // Inizialize the matrix
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
        transposed_serial[i] = new float[N];
        transposed_parallel[i] = new float[N];
        for (int j = 0; j < N; j++)
        {
            matrix[i][j] = (float)(rand() % 100 + 1);
            transposed_serial[i][j] = matrix[i][j];
            transposed_parallel[i][j] = matrix[i][j];
        }
    }

    // Inizialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Serial

    double start_serial = MPI_Wtime();

    if (!checkSym(matrix, N))
    {
        matTranspose(transposed_serial, N);
    }

    double end_serial = MPI_Wtime();
    double duration_serial = (end_serial - start_serial);
    double data_transferred = 4.0 * (N * N) * sizeof(float);
    double bandwidth_serial = data_transferred / (duration_serial * 1e9);

    // Row Wise

    double start_parallel = MPI_Wtime();

    if (!checkSym_MPI(matrix, N, size, rank))
    {
        matTransposeMPI(transposed_parallel, N, size, rank);
    }

    double end_parallel = MPI_Wtime();
    double duration_parallel = (end_parallel - start_parallel);
    double bandwidth_parallel = data_transferred / (duration_parallel * 1e9);

    // Printing result
    if (rank == 0)
    {
        // Serial
        cout << "Time taken by serial: " << duration_serial << " seconds" << endl;
        cout << "Effective Serial Bandwidth: " << bandwidth_serial << " GB/s" << endl
             << endl;

        // Parallel
        cout << "Time taken by RowWise: " << duration_parallel << " seconds" << endl;
        cout << "Effective Parallel Bandwidth: " << bandwidth_parallel << " GB/s" << endl
             << endl;

        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }

        cout << endl;

        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                cout << transposed_serial[i][j] << " ";
            }
            cout << endl;
        }

        cout << endl;

        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                cout << transposed_parallel[i][j] << " ";
            }
            cout << endl;
        }

        // Check
        cout << "Check transposed Matrix:" << check_transpose(transposed_serial, transposed_parallel, N) << endl;
    }

    // Finalize MPI
    MPI_Finalize();

    // Cleanup memory
    for (int i = 0; i < N; i++)
    {
        delete[] matrix[i];
        delete[] transposed_serial[i];
        delete[] transposed_parallel[i];
    }
    delete[] matrix;
    delete[] transposed_serial;
    delete[] transposed_parallel;

    return 0;
}
