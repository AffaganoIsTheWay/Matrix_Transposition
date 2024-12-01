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
    bool isSymmetric = 1;
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            if(matrix[i][j] != matrix[j][i]){
                isSymmetric = 0;
            }
        }
    }

    return isSymmetric;
}

int main(int argc, const char* argv[]) {
    int N = atoi( argv[1] );
    float** matrix = new float*[N];
    float** transposed = new float*[N];

    srand(time(NULL));
    
    //Inizialize the matrix
    for (int i = 0; i < N; i++){
        matrix[i] = new float[N];
        transposed[i] = new float[N];
        for (int j = 0; j < N; j++){
            matrix[i][j] = (float)(rand() % 100 + 1);
            transposed[i][j] = matrix[i][j];
        }
    }

    double start = omp_get_wtime();

    if(!checkSym(matrix, N)){
        matTranspose(transposed, N);
    }

    double end = omp_get_wtime();
    double duration = (end - start);
    double data_transferred = 4.0 * (N * N) * sizeof(int);
    double bandwidth = data_transferred / (duration * 1e9);

    std::cout << "Time taken by this oprimization flag(s): " << duration << " seconds" << endl;
    std::cout << "Effective Bandwidth: " << bandwidth << " GB/s" << endl << endl;

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
        delete[] transposed[i];
    }
    delete[] matrix;
    delete[] transposed;

    return 0;
}
