#ifndef UTILS_H
#define UTILS_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/* This is wrapped in a do-while loop so this macro function is compatible with
 * conditional statements that uses no braces. The braces of the do-while loop
 * enforces running the entire macro function instead of it's first line. */
#define MPI_CHECK(fn, is_exit)                                              \
    do {                                                                    \
        int _rc = (fn);                                                     \
        if (_rc != MPI_SUCCESS) {                                           \
            char _err_str[MPI_MAX_ERROR_STRING];                            \
            int _err_len;                                                   \
            MPI_Error_string(_rc, _err_str, &_err_len);                     \
            fprintf(stderr, "[%s,%d] MPI ERROR: %s\n", __FILE__, __LINE__,  \
                    _err_str);                                              \
            if (is_exit) {                                                  \
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);                    \
            }                                                               \
        }                                                                   \
    } while (0)

typedef struct WorldInfo {
    int rank;
    int size;
    MPI_Comm comm;
} WorldInfo;

#endif // UTILS_H
