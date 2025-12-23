#ifndef COMM_H
#define COMM_H

#include <mpi.h>

typedef struct WorldInfo {
    int rank;
    int size;
    MPI_Comm comm;
} WorldInfo;

#endif // COMM_H
