#ifndef IO_H
#define IO_H

#include <mpi.h>  // For inter-process communication and file I/O.
#include <stdbool.h>

struct WorldInfo;   // Forward declaration of MPI world info.

// Attempt to gather data from hopefully set XDG environment variables.
int find_data_dir_path(char** data_path, int* is_xdg);

// Ensure the data directory is present for data storage.
int ensure_data_dir(struct WorldInfo* info, char* path);

// Write execution time data of each process to a file.
int write_execution_time(struct WorldInfo* info, MPI_File file,
                         const bool is_header, int* total_written,
                         const char* op, double* exec_time);

#endif // IO_HPP
