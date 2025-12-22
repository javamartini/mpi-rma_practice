// Inter-process commmunication.
#include "comm.h"
#include <mpi.h>

// I/O-related headers and libraries.
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    WorldInfo info;
    MPI_Status status;
    const char* data_dir_path = "/home/x-jmartin7/.local/state/mpi-rma_practice/";
    char* data_file_path = "/home/x-jmartin7/.local/state/mpi-rma_practice/exec_times.csv";
    int rc = -1;

    MPI_Init(&argc, &argv);

    // Capture the current process rank and the total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &info.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &info.size);

    if (info.rank == 0) {
        rc = ensure_data_dir(&info, data_dir_path);
    }

    MPI_Bcast(&rc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rc != 0) {
        fprintf(stderr,
                "Rank %d aborting due to exit code of ensure_data_dir(): %d\n",
                info.rank, rc);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /* Split processes 0 and 1 into their own communicators for one-way
    * communication. */
    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, info.rank <= 1, info.rank, &comm);

    // Make a message to make availble via a window.
    float buf[10];
    if (info.rank == 0) {
        for (int i = 0; i < 10; i++) {
        buf[i] = 32.4;
        }
    }
    size_t buf_size = sizeof(buf) / sizeof(buf[0]);

    printf("rank: %d, buffer: ", info.rank);
    for (size_t i = 0; i < buf_size; i++) {
        printf("%f ", buf[i]);
    }
    printf("\n");

    // Create a window for both processes based on the buffer being put.
    MPI_Win win;
    if (info.rank == 0) {
        MPI_Win_create(MPI_BOTTOM, 0, sizeof(float), MPI_INFO_NULL, comm,
                       &win);
    } else {
        /* Expose the memory we want to put data into, and specify the current
        * byte size of the buffer. */
        MPI_Win_create(buf, buf_size * sizeof(float), sizeof(float),
                       MPI_INFO_NULL, comm, &win);
    }

    // Start timing.
    const double start_time = MPI_Wtime();

    // Put ten floats from rank 0's buffer into rank 1's window.
    MPI_Win_fence(0, win);
    if (info.rank == 0) {
        MPI_Put(buf, buf_size, MPI_FLOAT, 1, 0, buf_size, MPI_FLOAT, win);
    }

    // Complete the 'Put' operation.
    MPI_Win_fence(0, win);

    // Stop timing and calculate the execution time.
    const double end_time = MPI_Wtime();
    const double execution_time = end_time - start_time;
    printf("execution time: %f\n", execution_time);

    printf("rank: %d, buffer: ", info.rank);
    for (size_t i = 0; i < buf_size; i++) {
        printf("%f ", buf[i]);
    }
    printf("\n");

    MPI_Barrier(MPI_COMM_WORLD);

    // Open a file for storing collected data.
    MPI_File fh;
    rc = MPI_File_open(MPI_COMM_WORLD, data_file_path,
                       MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    if (rc != MPI_SUCCESS) {
        char err_str[MPI_MAX_ERROR_STRING];
        int err_len = -1;
        MPI_Error_string(rc, err_str, &err_len);
        fprintf(stderr, "MPI_File_open(): %s\n", err_str);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Free gathered resources.
    MPI_File_close(&fh);
    MPI_Win_free(&win);
    MPI_Comm_free(&comm);
    MPI_Finalize();

    return EXIT_SUCCESS;
    }
