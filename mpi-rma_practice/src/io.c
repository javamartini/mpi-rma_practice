#include "io.h"
#include "utils.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

// For filesystem traversal and operations.
#include <dirent.h>
#include <sys/stat.h>

// Ensure the data directory is present.
int ensure_data_dir(struct WorldInfo* info, const char* data_dir_path) {
    if (info->rank != 0) {
        return 0;
    }

    int rc = -1;
    bool found_dir = false;
    //struct dirent* dir_info;
    DIR* dir = opendir(data_dir_path);

    if (dir != NULL) {
        found_dir = true;
        closedir(dir);
    }

    if (!found_dir) {
        rc = mkdir(data_dir_path,
                   S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if (rc != 0) {
        perror("mkdir()");
        return 1;
        }
    }

    return 0;
}

// Write execution time data of each process to a file.
int write_execution_time(struct WorldInfo* info, MPI_File file,
                         const bool is_header, const char* op,
                         double* exec_time) {
    int rc = -1;

    // If the header has not yet been writte, write that.
    if (is_header) {
        if (info->rank == 0) {
            const char* header = "rank,operation,execution_time\n";
            MPI_CHECK(MPI_File_write(file, header, strlen(header), MPI_CHAR,
                                     MPI_STATUS_IGNORE), true);
        }

        MPI_Barrier(info->comm);
    }

    // The data to be written.
    char buffer[128];
    int line_len = snprintf(buffer, sizeof(buffer), "%3d,%-12s,%f\n",
                            info->rank, op, *exec_time);
    
    // Calculate the offset to skip writing over the header.
    MPI_Offset header_offset = 30;
    MPI_Offset rank_offset = header_offset + (info->rank * line_len);

    MPI_CHECK(MPI_File_write_at(file, rank_offset, buffer, line_len, MPI_CHAR,
                                MPI_STATUS_IGNORE), true);
    
    return 0;
}
