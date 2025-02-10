#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <mpi.h>

using namespace std;

// Function to transpose a matrix
void matTranspose(float **transposed, int N)
{
    float** local_matrix = new float*[N];
    for(int i = 0; i < N; i++){
        local_matrix[i] = new float[N];
        for(int j = 0; j < N; j++){ 
            local_matrix[i][j] = 0;
        }
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            local_matrix[i][j] = transposed[j][i];
        }
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            transposed[i][j] = local_matrix[i][j];
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

// Function to transpose a matrix using MPI
void matTransposeMPI(float **transposed, int N, int size, int rank) {
    int row_per_process = N / size;
    int row_start = rank * row_per_process;
    int row_end = (rank + 1) * row_per_process;

    float** local_matrix = new float*[N];
    for(int i = 0; i < N; i++){
        local_matrix[i] = new float[row_per_process];
        for(int j = 0; j < row_per_process; j++){
            local_matrix[i][j] = 0;
        }
    }

    for(int i = 0; i < N; i++){
        for(int j = 0, x = row_start; j < row_per_process; j++, x++){
            local_matrix[i][j] = transposed[x][i];
        }
    }

    int sizes[2] = {N, N};
    int subsizes[2] = {1, row_per_process};
    int start[2] = {0,0};
    MPI_Datatype type, subarrtype;
    MPI_Type_create_subarray(2, sizes, subsizes, start, MPI_ORDER_C, MPI_FLOAT, &type);
    MPI_Type_create_resized(type, 0, row_per_process*sizeof(float), &subarrtype);
    MPI_Type_commit(&subarrtype);

    int sendcounts[row_per_process];
    int displs[row_per_process];

    if (rank == 0) {
        int disp = 0;
        for(int i = 0; i < row_per_process; i++){
            sendcounts[i] = 1;
            displs[i] = disp;
            disp += 1;
        }
    }

    for(int i = 0; i < N; i++){
        MPI_Gatherv(&(local_matrix[i][0]), row_per_process, MPI_FLOAT,
                &(transposed[i][row_start]), sendcounts, displs, subarrtype, 0, MPI_COMM_WORLD);
    }
}

bool checkSymMPI(float **matrix, int N, int size, int rank)
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

    ofstream ResultFile;
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {
        ResultFile.open("result.csv", ios_base::app);
    }

    int Matrix_size[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    for (int i = 0; size <= Matrix_size[i]; i++)
    {

        if (Matrix_size[i] % size != 0) {
            break;
        }

        float **matrix = new float *[Matrix_size[i]];
        float **transposed_serial = new float *[Matrix_size[i]];
        float **transposed_parallel = new float *[Matrix_size[i]];

        srand(time(0));

        // Inizialize the matrix
        for (int j = 0; j < Matrix_size[i]; j++)
        {
            matrix[j] = new float[Matrix_size[i]];
            transposed_serial[j] = new float[Matrix_size[i]];
            transposed_parallel[j] = new float[Matrix_size[i]];
            for (int k = 0; k < Matrix_size[i]; k++)
            {
                matrix[j][k] = (float)(rand() % 100 + 1);
                transposed_serial[j][k] = matrix[j][k];
                transposed_parallel[j][k] = matrix[j][k];
            }
        }

        if(rank == 0) {
            ResultFile << "Matrix size" << endl
                       << Matrix_size[i] << endl;

            ResultFile << "Cores" << "," << "Serial Time" << "," << "Parallel Time" << "," << "SpeedUp" << endl;
        }

        double start_serial = MPI_Wtime();

        if (!checkSym(matrix, Matrix_size[i]))
        {
            matTranspose(transposed_serial, Matrix_size[i]);
        }

        double end_serial = MPI_Wtime();
        double duration_serial = (end_serial - start_serial);

        for (int k = 0; k < 10; k++)
        {
            double start_parallel = MPI_Wtime();

            if (!checkSymMPI(matrix, Matrix_size[i], size, rank))
            {
                matTransposeMPI(transposed_parallel, Matrix_size[i], size, rank);
            }
            
            double end_parallel = MPI_Wtime();
            double duration_parallel = (end_parallel - start_parallel);
            ResultFile << size << "," << duration_serial << "," << duration_parallel << "," << duration_serial / duration_parallel << endl;
        }

        // Cleanup memory
        for (int j = 0; j < Matrix_size[i]; j++)
        {
            delete[] matrix[j];
            delete[] transposed_serial[j];
            delete[] transposed_parallel[j];
        }
        delete[] matrix;
        delete[] transposed_serial;
        delete[] transposed_parallel;
    }

    if (rank == 0) {
        ResultFile.close();
    }

    return 0;
}