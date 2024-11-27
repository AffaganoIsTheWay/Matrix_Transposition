#include <iostream>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function to transpose a matrix
void matTranspose(float** transposed, int N) {
    int temp;
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            if(transposed[i][j] != transposed[j][i]){
                temp = transposed[i][j];
                transposed[i][j] = transposed[j][i];
                transposed[j][i] = temp;
            }
        }
    }
}

bool checkSym(float** matrix, int N){
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            if(matrix[i][j] != matrix[j][i]){
                return false;
            }
        }
    }

    return true;
}

bool check_transpose(float** transposed_serial,float** transposed_parallel, int N){
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
void matTransposeOMP(float** transposed, int N) {
    int temp;
    // Parallelize the inner loop
    for (int i = 0; i < N; i++) {
        #pragma omp parallel for private(temp) schedule(guided)
        for (int j = i + 1; j < N; j++) {
            if(transposed[i][j] != transposed[j][i]) {
                temp = transposed[i][j];
                transposed[i][j] = transposed[j][i];
                transposed[j][i] = temp;
            }
        }
    }
}

bool checkSymOMP(float** matrix, int N) {
    bool isSymmetric = true;

    #pragma omp parallel for reduction(&&: isSymmetric)
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (matrix[i][j] != matrix[j][i]) {
                isSymmetric = false;
            }
        }
    }

    return isSymmetric;
}
int main(int argc, const char* argv[]) {
    int N = atoi( argv[1] );
    float** matrix = new float*[N];
    float** transposed_serial = new float*[N];
    float** transposed_parallel = new float*[N];

    srand(time(NULL));
    
    //Inizialize the matrix
    for (int i = 0; i < N; i++){
        matrix[i] = new float[N];
        transposed_serial[i] = new float[N];
        transposed_parallel[i] = new float[N];
        for (int j = 0; j < N; j++){
            matrix[i][j] = (float)(rand() % 100 + 1);
            transposed_serial[i][j] = matrix[i][j];
            transposed_parallel[i][j] = matrix[i][j];
        }
    }

    double start_serial = omp_get_wtime();

    if(!checkSym(matrix, N)){
        matTranspose(transposed_serial, N);
    }

    double end_serial = omp_get_wtime();
    double duration_serial = (end_serial - start_serial);
    double data_transferred = 4.0 * (N * N) * sizeof(float);
    double bandwidth_serial = data_transferred / (duration_serial * 1e9);

    std::cout << "Time taken by serial: " << duration_serial << " seconds" << endl;
    std::cout << "Effective Serial Bandwidth: " << bandwidth_serial << " GB/s" << endl << endl;

    double start_parallel = omp_get_wtime();

    if(!checkSymOMP(matrix, N)){
        matTransposeOMP(transposed_parallel, N);
    }

    double end_parallel = omp_get_wtime();
    double duration_parallel = (end_parallel - start_parallel);
    double bandwidth_parallel = data_transferred / (duration_parallel * 1e9);

    cout << "Time taken by parallel Static: " << duration_parallel << " seconds" << endl;
    cout << "Effective Parallel Bandwidth: " << bandwidth_parallel << " GB/s" << endl << endl;

    cout << "Check transposed Matrix:" << check_transpose(transposed_serial, transposed_parallel, N) << endl;

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
        delete[] transposed_serial[i];
        delete[] transposed_parallel[i];
    }
    delete[] matrix;
    delete[] transposed_serial;
    delete[] transposed_parallel;

    return 0;
}
