#include <iostream>
#include <mpi.h>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function to transpose a matrix
void matTranspose(float *transposed, int N) {
    float temp;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (transposed[i * N + j] != transposed[j * N + i]) {
                temp = transposed[i * N + j];
                transposed[i * N + j] = transposed[j * N + i];
                transposed[j * N + i] = temp;
            }
        }
    }
}

bool checkSym(float *matrix, int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (matrix[i * N + j] != matrix[j * N + i])
            {
                return false;
            }
        }
    }

    return true;
}

bool check_transpose(float *transposed_serial, float *transposed_parallel, int N)
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (transposed_serial[i * N + j] != transposed_parallel[i * N + j])
            {
                return false;
            }
        }
    }

    return true;
}

// Function to transpose a matrix using MPI
void matTransposeMPI(float *transposed, int N, int size, int rank) {
    int row_per_process = N / size;
    int row_start = rank * row_per_process;
    int row_end = (rank + 1) * row_per_process;

    float *local_matrix = new float[N*row_per_process];
    
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < row_per_process; j++) {
            local_matrix[i * row_per_process + j] = transposed[(row_start + j) * N + i];
        }
    }
    
    for(int i = 0; i < N; i++){
        MPI_Gather(&(local_matrix[i * row_per_process]), row_per_process, MPI_FLOAT,
            &(transposed[i * N + row_start]), row_per_process, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

}

void printMatrix(float* matrix, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << matrix[i * n + j] << " "; 
        }
        cout << endl;
    }
}

bool checkSym_MPI(float *matrix, int N, int size, int rank)
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
            if (matrix[i * N + j] != matrix[j * N + i])
            {
                local_result = false;
                break;
            }
        }
        if (!local_result)
            break;
    }

    bool global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

    return global_result;
}

int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);
    float *matrix = new float [N*N];
    float *transposed_serial = new float [N*N];
    float *transposed_parallel = new float [N*N];

    srand(time(NULL));

    // Inizialize the matrix
    for (int i = 0; i < N * N; i++) {
        matrix[i] = (float)(rand() % 100 + 1);
        transposed_serial[i] = matrix[i];
        transposed_parallel[i] = matrix[i];
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

        // Check
        cout << "Check transposed Matrix:" << check_transpose(transposed_serial, transposed_parallel, N) << endl;
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
