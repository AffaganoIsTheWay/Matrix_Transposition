#include <iostream>
#include <omp.h>

using namespace std;

#define MAX_ROWS 15000 // Maximum number of rows
#define MAX_COLS 15000 // Maximum number of columns

// Function to transpose a matrix
void transpose_serial(int** matrix, int** transposed) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
}



// Function to transpose a matrix using OpenMP
void transpose_parallel_collapse_static(int** matrix,int** transposed) {
    // Parallelize the outer loop with static schedule
#pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLS; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
}

bool check_transpose(int** transposed_serial,int** transposed_parallel){
    for (int i = 0; i < MAX_COLS; ++i) {
        for (int j = 0; j < MAX_ROWS; ++j) {
            if(transposed_serial[i][j] != transposed_parallel[i][j]){
                return false;
            }
        }
    }

    return true;
}

int main() {
    int** matrix = new int*[MAX_ROWS];
    int** transposed_serial = new int*[MAX_COLS];
    int** transposed_parallel = new int*[MAX_COLS];

    double start_parallel, end_parallel, duration_parallel, bandwidth_parallel;

    
    //Inizialize the matrix
    for (int i = 0; i < MAX_ROWS; i++){
        matrix[i] = new int[MAX_COLS];
        for (int j = 0; j < MAX_COLS; j++){
            matrix[i][j] = i + j;
        }
        
    }

    //Inizialize trasposed
    for (int i = 0; i < MAX_COLS; i++){
        transposed_serial[i] = new int[MAX_ROWS];
        transposed_parallel[i] = new int[MAX_ROWS];
        
    }

    double start_serial = omp_get_wtime();

    transpose_serial(matrix, transposed_serial);

    double end_serial = omp_get_wtime();
    double duration_serial = (end_serial - start_serial);
    double data_transferred = 2.0 * MAX_COLS * MAX_ROWS * sizeof(int);
    double bandwidth_serial = data_transferred / (duration_serial * 1e9);

    std::cout << "Time taken by serial: " << duration_serial << " seconds" << endl;
    std::cout << "Effective Serial Bandwidth: " << bandwidth_serial << " GB/s" << endl << endl;

    start_parallel = omp_get_wtime();

    transpose_parallel_collapse_static(matrix, transposed_parallel);

    end_parallel = omp_get_wtime();
    duration_parallel = (end_parallel - start_parallel);
    bandwidth_parallel = data_transferred / (duration_parallel * 1e9);

    cout << "Time taken by parallel Collapse Static: " << duration_parallel << " seconds" << endl;
    cout << "Effective Parallel Bandwidth: " << bandwidth_parallel << " GB/s" << endl << endl;

    cout << "Check transposed Matrix:" << check_transpose(transposed_serial, transposed_parallel) << endl;

    // Cleanup memory
    for (int i = 0; i < MAX_ROWS; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;

    for (int i = 0; i < MAX_COLS; i++) {
        delete[] transposed_serial[i];
        delete[] transposed_parallel[i];
    }
    delete[] transposed_serial;
    delete[] transposed_parallel;

    return 0;
}
