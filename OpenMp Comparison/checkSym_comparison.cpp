#include <iostream>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;

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

bool checkSym_reduction(float** matrix, int N) {
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


bool checkSym_atomic(float** matrix, int N) {
    bool isSymmetric = true;

    #pragma omp parallel
    {
        bool localSymmetric = true; // Thread-local flag

        #pragma omp for nowait
        for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
                if (matrix[i][j] != matrix[j][i]) {
                    localSymmetric = false;
                }
            }
        }

        // Combine the result from each thread
        #pragma omp atomic
        isSymmetric &= localSymmetric;
    }

    return isSymmetric;
}

int main(int argc, const char* argv[]) {
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

    double start_serial = omp_get_wtime();

    cout << "serial: " << checkSym(matrix, N) << endl;

    double end_serial = omp_get_wtime();
    double duration_serial = (end_serial - start_serial);

    std::cout << "Time taken by serial: " << duration_serial << " seconds" << endl;

    double start_reduction = omp_get_wtime();

    cout << "reduction: " << checkSym_reduction(matrix, N) << endl;

    double end_reduction = omp_get_wtime();
    double duration_reduction = (end_reduction - start_reduction);

    cout << "Time taken by reduction: " << duration_reduction << " seconds" << endl;

    double start_atomic = omp_get_wtime();

    cout << "atomic: " << checkSym_atomic(matrix, N) << endl;

    double end_atomic = omp_get_wtime();
    double duration_atomic = (end_atomic - start_atomic);

    std::cout << "Time taken by atomic: " << duration_atomic << " seconds" << endl;

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;

    return 0;
}
