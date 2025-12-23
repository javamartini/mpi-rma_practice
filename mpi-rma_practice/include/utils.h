#ifndef UTILS_H
#define UTILS_H

#include <mpi.h>

typedef struct WorldInfo {
    int rank;
    int size;
    MPI_Comm comm;
} WorldInfo;

#endif // UTILS_H
