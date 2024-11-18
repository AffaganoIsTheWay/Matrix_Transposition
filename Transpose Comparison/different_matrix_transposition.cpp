#include <iostream>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function to transpose a matrix cloning the matrix inverting rows and colums
void transpose_One(float** matrix, float** transposed, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            transposed[j][i] = matrix[i][j];
        }
    }
}

// Function to transpose swappig value
void transpose_Two(float** transposed, int N) {
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
    float** transposed_One = new float*[N];
    float** transposed_Two = new float*[N];

    srand(time(NULL));
    
    //Inizialize the matrix
    for (int i = 0; i < N; i++){
        matrix[i] = new float[N];
        transposed_One[i] = new float[N];
        transposed_Two[i] = new float[N];
        for (int j = 0; j < N; j++){
            matrix[i][j] = (float)(rand() % 100 + 1);
            transposed_Two[i][j] = matrix[i][j];
        }
        
    }


    double start_one = omp_get_wtime();

    transpose_One(matrix, transposed_One, N);

    double end_one = omp_get_wtime();
    double duration_one = (end_one - start_one);
    double data_transferred = 4.0 * (N * N) * sizeof(float);
    double bandwidth_one = data_transferred / (duration_one * 1e9);

    std::cout << "Time taken by new assignment transpose: " << duration_one << " seconds" << endl;
    std::cout << "Effective Bandwidth: " << bandwidth_one << " GB/s" << endl << endl;


    double start_two = omp_get_wtime();

    transpose_Two(transposed_Two, N);

    double end_two = omp_get_wtime();
    double duration_two = (end_two - start_two);
    double bandwidth_two = data_transferred / (duration_two * 1e9);

    cout << "Time taken by swapping transpose: " << duration_two << " seconds" << endl;
    cout << "Effective Bandwidth: " << bandwidth_two << " GB/s" << endl << endl;

    cout << "Check transposition: " << checkTranspose(transposed_One, transposed_Two, N) << endl;

    // Cleanup memory
    for (int i = 0; i < N; i++) {
        delete[] matrix[i];
        delete[] transposed_One[i];
        delete[] transposed_Two[i];
    }
    delete[] matrix;
    delete[] transposed_One;
    delete[] transposed_Two;

    return 0;
}
