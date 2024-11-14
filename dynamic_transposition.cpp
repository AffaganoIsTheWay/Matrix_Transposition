#include <iostream>
#include <ctime>
#include <sys/time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

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
#ifdef _OPENMP
void transpose_parallel_dynamic(int** matrix,int** transposed) {
    // Parallelize the outer loop with dynamic schedule
#pragma omp parallel for schedule(dynamic)
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
#endif

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

    clock_t start_serial = clock();

    transpose_serial(matrix, transposed_serial);

    clock_t end_serial = clock();
    double duration_serial = static_cast<double>(end_serial - start_serial) / CLOCKS_PER_SEC;
    double data_transferred = 2.0 * MAX_COLS * MAX_ROWS * sizeof(int);
    double bandwidth_serial = data_transferred / (duration_serial * 1e9);

    std::cout << "Time taken by serial: " << duration_serial << " seconds" << endl;
    std::cout << "Effective Serial Bandwidth: " << bandwidth_serial << " GB/s" << endl << endl;


#ifdef _OPENMP
    start_parallel = omp_get_wtime();

    transpose_parallel_dynamic(matrix, transposed_parallel);

    end_parallel = omp_get_wtime();
    duration_parallel = (end_parallel - start_parallel);
    bandwidth_parallel = data_transferred / (duration_parallel * 1e9);

    cout << "Time taken by parallel Dynamic: " << duration_parallel << " seconds" << endl;
    cout << "Effective Parallel Bandwidth: " << bandwidth_parallel << " GB/s" << endl << endl;

    cout << "Check transposed Matrix:" << check_transpose(transposed_serial, transposed_parallel) << endl;
#endif

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
