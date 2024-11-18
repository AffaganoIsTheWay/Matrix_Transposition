#include <iostream>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function to transpose a matrix
void transpose_serial(int** matrix, int** transposed, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
}

bool check_transpose(int** transposed_serial,int** transposed_parallel, int N){
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if(transposed_serial[i][j] != transposed_parallel[i][j]){
                return false;
            }
        }
    }

    return true;
}

// Function to transpose a matrix using OpenMP
void transpose_parallel_dynamic(int** matrix,int** transposed, int N) {
    // Parallelize the outer loop with dynamic schedule
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
}

int main(int argc, const char* argv[]) {
    int N = atoi( argv[1] );
    int** matrix = new int*[N];
    int** transposed_serial = new int*[N];
    int** transposed_parallel = new int*[N];

    double start_parallel, end_parallel, duration_parallel, bandwidth_parallel;

    srand(time(NULL));
    
    //Inizialize the matrix
    for (int i = 0; i < N; i++){
        matrix[i] = new int[N];
        transposed_serial[i] = new int[N];
        transposed_parallel[i] = new int[N];
        for (int j = 0; j < N; j++){
            matrix[i][j] = rand() % 100 + 1;
        }
        
    }

    double start_serial = omp_get_wtime();

    transpose_serial(matrix, transposed_serial, N);

    double end_serial = omp_get_wtime();
    double duration_serial = (end_serial - start_serial);
    double data_transferred = 2.0 * N * N * sizeof(int);
    double bandwidth_serial = data_transferred / (duration_serial * 1e9);

    std::cout << "Time taken by serial: " << duration_serial << " seconds" << endl;
    std::cout << "Effective Serial Bandwidth: " << bandwidth_serial << " GB/s" << endl << endl;

    start_parallel = omp_get_wtime();

    transpose_parallel_dynamic(matrix, transposed_parallel, N);

    end_parallel = omp_get_wtime();
    duration_parallel = (end_parallel - start_parallel);
    bandwidth_parallel = data_transferred / (duration_parallel * 1e9);

    cout << "Time taken by parallel Dynamic: " << duration_parallel << " seconds" << endl;
    cout << "Effective Parallel Bandwidth: " << bandwidth_parallel << " GB/s" << endl << endl;

    cout << "Check transposed Matrix:" << check_transpose(transposed_serial, transposed_parallel, N) << endl;

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;

    for (int i = 0; i < N; i++) {
        delete[] transposed_serial[i];
        delete[] transposed_parallel[i];
    }
    delete[] transposed_serial;
    delete[] transposed_parallel;

    return 0;
}
