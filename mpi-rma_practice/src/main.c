// Inter-process commmunication.
#include "utils.h"
#include "net.h"
#include <mpi.h>

// I/O-related headers and libraries.
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

int main(int argc, char *argv[]) {
    WorldInfo info;
    MPI_Status status;
    char data_dir_path[PATH_MAX] = "";
    char data_file_path[PATH_MAX] = "";
    int rc = -1;

    MPI_CHECK(MPI_Init(&argc, &argv), true);

    // Capture the current process rank and the total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &info.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &info.size);

    if (info.rank == 0) {
        rc = ensure_data_dir(&info, data_dir_path);
    }
    MPI_CHECK(MPI_Bcast(&rc, 1, MPI_INT, 0, MPI_COMM_WORLD), true);
    if (rc != 0) {
        fprintf(stderr,
                "Rank %d aborting due to exit code of ensure_data_dir(): %d\n",
                info.rank, rc);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Let everyone know the actual data directory path.
    MPI_CHECK(MPI_Bcast(data_dir_path, PATH_MAX, MPI_CHAR, 0,
                        MPI_COMM_WORLD),
              true);

    // Build the path to the actual file.
    snprintf(data_file_path, PATH_MAX, "%s/exec_times.csv", data_dir_path);

    MPI_Barrier(MPI_COMM_WORLD);

    /* Split processes 0 and 1 into their own communicators for one-way
    * communication. */
    MPI_CHECK(MPI_Comm_split(MPI_COMM_WORLD, info.rank <= 1, info.rank,
                             &info.comm),
              true);

    // Open a file for storing collected data.
    MPI_File fh;
    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, data_file_path,
                            MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL,
                            &fh),
              true);

    int total_written = 0;
    for (int j = 1; j <= 100; j++) {
        MPI_Barrier(MPI_COMM_WORLD);
        
        // Make a message to make availble via a window.
        float buf[MSG_SIZE];
        if (info.rank == 0) {
            for (int i = 0; i < MSG_SIZE; i++) {
                buf[i] = 32.4;
            }
        } else {
            for (int i = 0; i < MSG_SIZE; i++) {
                buf[i] = 0;
            }
        }
        size_t buf_size = sizeof(buf) / sizeof(buf[0]);

        printf("rank: %d, buffer: ", info.rank);
        for (size_t i = 0; i < buf_size; i++) {
            printf("%f ", buf[i]);
        }
        printf("\n");

        double exec_time = 0.000000;
        rma(buf, buf_size, &info, &exec_time);

        printf("rank: %d, buffer: ", info.rank);
        for (size_t i = 0; i < buf_size; i++) {
            printf("%f ", buf[i]);
        }
        printf("\n");

        MPI_Barrier(MPI_COMM_WORLD);

        if (j == 0) {
            rc = write_execution_time(&info, fh, true, &total_written,
                                      "rma_broadcast", &exec_time, j);
            if (rc != 0) {
                fprintf(stderr,
                        "Rank %d aborting due to exit code of ensure_data_dir(): %d\n",
                        info.rank, rc);
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }
        } else {
            rc = write_execution_time(&info, fh, false, &total_written,
                                      "rma_broadcast", &exec_time, j);
            if (rc != 0) {
                fprintf(stderr,
                        "Rank %d aborting due to exit code of ensure_data_dir(): %d\n",
                        info.rank, rc);
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }
        }
    }

    // Free gathered resources.
    MPI_CHECK(MPI_File_close(&fh), true);
    MPI_CHECK(MPI_Comm_free(&info.comm), true);
    MPI_CHECK(MPI_Finalize(), true);

    return EXIT_SUCCESS;
}
