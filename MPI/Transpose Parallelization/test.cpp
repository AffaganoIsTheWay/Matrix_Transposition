#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <stdexcept>

void printMatrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << matrix[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows = std::atoi(argv[1]);
    int cols = std::atoi(argv[1]);

    // Broadcast dimensions to all processes
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);

    const int total_elements = rows * cols;

    // Validate matrix size and process count
    if (total_elements % size != 0) {
        if (rank == 0) {
            std::cerr << "Error: Matrix size (" << total_elements 
                      << " elements) must be divisible by number of processes (" 
                      << size << ")" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    // Initialize matrices using dynamic arrays
    int* matrix = nullptr;
    int* transposed_matrix = nullptr;
    
    if (rank == 0) {
        matrix = new int[total_elements];
        transposed_matrix = new int[total_elements];
        
        // Initialize the matrix
        for (int i = 0; i < total_elements; i++) {
            matrix[i] = i + 1;  // Sample data
        }

        std::cout << "Original Matrix (" << rows << "x" << cols << "):" << std::endl;
        printMatrix(matrix, rows, cols);
    } else {
        transposed_matrix = new int[total_elements];
    }

    // Calculate elements per process
    int elements_per_process = total_elements / size;
    int* local_data = new int[elements_per_process];
    int* local_transposed = new int[elements_per_process];

    // Scatter the matrix among processes
    MPI_Scatter(matrix, elements_per_process, MPI_INT,
                local_data, elements_per_process, MPI_INT,
                0, MPI_COMM_WORLD);

    // Perform local transposition calculations
    for (int i = 0; i < elements_per_process; i++) {
        // Calculate original position
        int global_index = rank * elements_per_process + i;
        int original_row = global_index / cols;
        int original_col = global_index % cols;
        
        // Calculate transposed position
        int new_index = original_col * rows + original_row;
        local_transposed[i] = local_data[i];
    }

    // Gather the transposed pieces
    MPI_Gather(local_transposed, elements_per_process, MPI_INT,
               transposed_matrix, elements_per_process, MPI_INT,
               0, MPI_COMM_WORLD);

    // Print the result in root process
    if (rank == 0) {
        std::cout << "\nTransposed Matrix (" << cols << "x" << rows << "):" << std::endl;
        printMatrix(transposed_matrix, cols, rows);
    }

    // Clean up memory
    delete[] local_data;
    delete[] local_transposed;
    if (rank == 0) {
        delete[] matrix;
    }
    delete[] transposed_matrix;

    MPI_Finalize();
    return 0;
}