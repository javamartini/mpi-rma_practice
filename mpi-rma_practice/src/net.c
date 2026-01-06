#include "net.h"

// Inter-process communication.
#include "utils.h"
#include <mpi.h>

#include <stdbool.h>

// Pass a message using Remote Memory Access.
int rma(float* buf, size_t buf_size, struct WorldInfo* info, double* execution_time) {
    // Create a window for both processes based on the buffer being put.
    MPI_Win win;
    if (info->rank == 0) {
        MPI_CHECK(MPI_Win_create(MPI_BOTTOM, 0, sizeof(float), MPI_INFO_NULL,
                                 info->comm, &win),
                  true);
    } else {
        /* Expose the memory we want to put data into, and specify the current
        * byte size of the buffer. */
        MPI_CHECK(MPI_Win_create(buf, buf_size * sizeof(float), sizeof(float),
                                 MPI_INFO_NULL, info->comm, &win),
                  true);
    }

    // Start timing.
    const double start_time = MPI_Wtime();

    // Put ten floats from rank 0's buffer into rank 1's window.
    MPI_CHECK(MPI_Win_fence(0, win), false);
    if (info->rank == 0) {
        MPI_CHECK(MPI_Put(buf, buf_size, MPI_FLOAT, 1, 0, buf_size, MPI_FLOAT,
                          win),
                  false);
    }

    // Complete the 'Put' operation.
    MPI_CHECK(MPI_Win_fence(0, win), false);

    // Stop timing and calculate the execution time.
    const double end_time = MPI_Wtime();
    double exec_time = end_time - start_time;

    if (execution_time != NULL) {
        *execution_time = exec_time;
    }

    MPI_CHECK(MPI_Win_free(&win), true);

    return 0;
}
