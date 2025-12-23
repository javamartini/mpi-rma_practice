#ifndef IO_H
#define IO_H

#include <mpi.h>  // For inter-process communication and file I/O.
#include <stdbool.h>

struct WorldInfo;   // Forward declaration of MPI world info.

// Ensure the data directory is present for data storage.
int ensure_data_dir(struct WorldInfo* info, const char* data_dir_path);

// Write execution time data of each process to a file.
int write_execution_time(struct WorldInfo* info, MPI_File file,
                         const bool is_header, const char* op,
                         double* exec_time);

#endif // IO_HPP
