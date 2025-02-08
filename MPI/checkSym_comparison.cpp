#include <iostream>
#include <mpi.h>
#include <ctime>
#include <cstdlib>

using namespace std;

bool checkSym(float** matrix, int N){
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            if(matrix[i][j] != matrix[j][i]){
                return 0;
            }
        }
    }

    return 1;
}

bool checkSym_RowWise(float **matrix, int N, int size, int rank)
{
    bool local_result = true;
    int rows_per_process = N / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? N : start_row + rows_per_process;

    // Each process checks its portion of the matrix
    for (int i = start_row; i < end_row; i++)
    {
        for (int j = i + 1; j < N; j++)
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



bool checkSym_ColumnWise(float **matrix, int N, int size, int rank)
{
    bool local_result = true;
    int cols_per_process = N / size;
    int start_col = rank * cols_per_process;
    int end_col = (rank == size - 1) ? N : start_col + cols_per_process;

    // Each process checks its portion of the matrix
    for (int i = 0; i < N; i++)
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

bool checkSym_BlockWise(float **matrix, int N, int size, int rank)
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

int main(int argc, char* argv[]) {
    int N = atoi( argv[1] );
    float** matrix = new float*[N];

    srand(time(NULL));
    
    //Inizialize the matrix
    for (int i = 0; i < N; i++){
        matrix[i] = new float[N];
        for (int j = 0; j < N; j++){
            matrix[i][j] = (float)(rand() % 100 + 1);
        }
    }

    //Inizialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Serial

    double start_serial = MPI_Wtime();

    bool serial = checkSym(matrix, N);

    double end_serial = MPI_Wtime();
    double duration_serial = (end_serial - start_serial);

    //Row Wise

    double start_row = MPI_Wtime();

    bool row = checkSym_RowWise(matrix, N, size, rank);

    double end_row = MPI_Wtime();
    double duration_row = (end_row - start_row);

    //Column Wise

    double start_column = MPI_Wtime();

    bool column = checkSym_ColumnWise(matrix, N, size, rank);

    double end_column = MPI_Wtime();
    double duration_column = (end_column - start_column);

    //Block Wise

    double start_block = MPI_Wtime();

    bool block = checkSym_BlockWise(matrix, N, size, rank);

    double end_block = MPI_Wtime();
    double duration_block = (end_block - start_block);

    //Printing result
    if(rank == 0){
        //Serial
        cout << "serial: " << serial << endl;
        std::cout << "Time taken by serial: " << duration_serial << " seconds" << endl;

        //Row Wise
        cout << "Row Wise: " << row << endl;
        cout << "Time taken by row wise: " << duration_row << " seconds" << endl;

        //Column Wise
        cout << "Column Wise: " << column << endl;
        std::cout << "Time taken by column wise: " << duration_column << " seconds" << endl;

        //Block Wise
        cout << "Block Wise: " << block << endl;
        std::cout << "Time taken by block wise: " << duration_block << " seconds" << endl;
    }

    //Finalize MPI
    MPI_Finalize();

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;

    return 0;
}
