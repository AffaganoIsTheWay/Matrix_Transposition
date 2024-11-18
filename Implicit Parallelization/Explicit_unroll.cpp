#include <iostream>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function to transpose a matrix
void matTransposeImp(float** transposed, int N) {
    int temp, j;
    for (int i = 0; i < N; ++i) {
        for (j = i+1; j+4 < N; j += 4) {
            // Unrolled body for 4 elements at a time
            if (transposed[i][j] != transposed[j][i]) {
                temp = transposed[i][j];
                transposed[i][j] = transposed[j][i];
                transposed[j][i] = temp;
            }

            if (transposed[i][j + 1] != transposed[j + 1][i]) {
                temp = transposed[i][j + 1];
                transposed[i][j + 1] = transposed[j + 1][i];
                transposed[j + 1][i] = temp;
            }

            if (transposed[i][j + 2] != transposed[j + 2][i]) {
                temp = transposed[i][j + 2];
                transposed[i][j + 2] = transposed[j + 2][i];
                transposed[j + 2][i] = temp;
            }

            if (transposed[i][j + 3] != transposed[j + 3][i]) {
                temp = transposed[i][j + 3];
                transposed[i][j + 3] = transposed[j + 3][i];
                transposed[j + 3][i] = temp;
            }
        }
        for(j; j < N; j++){
            if (transposed[i][j] != transposed[j][i]) {
                temp = transposed[i][j];
                transposed[i][j] = transposed[j][i];
                transposed[j][i] = temp;
            }
        }
    }
}

bool checkSymImp(float** matrix, int N){
    int j;
    for (int i = 0; i < N; i++) {
        for (j = i+1; j+4 < N; j += 4) {
            if(matrix[i][j] != matrix[j][i]){
                return false;
            }
            if(matrix[i][j + 1] != matrix[j + 1][i]){
                return false;
            }
            if(matrix[i][j + 2] != matrix[j + 2][i]){
                return false;
            }
            if(matrix[i][j + 3] != matrix[j + 3][i]){
                return false;
            }
        }
        for (j; j < N; j++){
            if(matrix[i][j] != matrix[j][i]){
                return false;
            }
        }
    }

    return true;
}

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

bool checkTranspose(float** transposed_one, float** transposed_two, int N){
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            if(transposed_one[i][j] != transposed_two[i][j]){
                return false;
            }
        }
    }

    return true;
}

int main(int argc, const char* argv[]) {
    int N = atoi( argv[1] );
    float** matrix = new float*[N];
    float** transposed = new float*[N];
    float** transposed_IMP = new float*[N];

    srand(time(NULL));
    
    //Inizialize the matrix
    for (int i = 0; i < N; i++){
        matrix[i] = new float[N];
        transposed[i] = new float[N];
        transposed_IMP[i] = new float[N];
        for (int j = 0; j < N; j++){
            matrix[i][j] = (float)(rand() % 100 + 1);
            transposed[i][j] = matrix[i][j];
            transposed_IMP[i][j] = matrix[i][j];
        }
    }

    double start = omp_get_wtime();

    if(!checkSym(matrix, N)){
        matTranspose(transposed, N);
    }

    double end = omp_get_wtime();
    double duration = (end - start);
    double data_transferred = 4.0 * (N * N) * sizeof(float);
    double bandwidth = data_transferred / (duration * 1e9);

    std::cout << "Time taken by serial: " << duration << " seconds" << endl;
    std::cout << "Effective Serial Bandwidth: " << bandwidth << " GB/s" << endl << endl;

    double start_imp = omp_get_wtime();

    if(!checkSymImp(matrix, N)){
        matTransposeImp(transposed_IMP, N);
    }

    double end_imp = omp_get_wtime();
    double duration_imp = (end_imp - start_imp);
    double bandwidth_imp = data_transferred / (duration_imp * 1e9);

    std::cout << "Time taken by IMP: " << duration_imp << " seconds" << endl;
    std::cout << "Effective IMP Bandwidth: " << bandwidth_imp << " GB/s" << endl << endl;

    cout << "Check transposition: " << checkTranspose(transposed, transposed_IMP, N) << endl;

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
        delete[] transposed[i];
    }
    delete[] matrix;
    delete[] transposed;

    return 0;
}
