#include "io.h"
#include "utils.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

// For filesystem traversal and operations.
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

// Attempt to gather data from hopefully set XDG environment variables.
int find_data_dir_path(char** data_path, int* is_xdg) {
    char* env = NULL;
    
    // Attempt to first gather the XDG specification of the data directory.
    env = getenv("XDG_DATA_HOME");
    if (env && env[0] != '\0') {
        *is_xdg = 0;
        *data_path = env;
        return 0;
    }

    // If unable, fallback to the HOME environment variable.
    env = getenv("HOME");
    if (env && env[0] != '\0') {
        *data_path = env;
        return 0;
    }

    return 1;
}

// Ensure the data directory is present.
int ensure_data_dir(struct WorldInfo* info, char* path) {
    if (info->rank != 0) {
        return 0;
    }

    int rc = -1;
    int is_xdg = 1;

    // Gather the path to the data directory.
    char* base_dir = NULL;
    rc = find_data_dir_path(&base_dir, &is_xdg);
    if (rc != 0) {
        fprintf(stderr, "Unable to find the data directory path\n");
        return 1;
    }

    // Create the full directory path.
    if (is_xdg == 0) {
        snprintf(path, PATH_MAX, "%s/mpi-rma_practice", base_dir);
    } else {
        snprintf(path, PATH_MAX, "%s/.mpi-rma_practice", base_dir);
    }

    bool found_dir = false;
    DIR* dir = opendir(path);
    if (dir != NULL) {
        found_dir = true;
        closedir(dir);
    }

    if (!found_dir) {
        rc = mkdir(path,
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
                         const bool is_header, int* total_written,
                         const char* op, double* exec_time, int round) {
    int rc = -1;

    // If the header has not yet been writte, write that.
    if (is_header) {
        if (info->rank == 0) {
            const char* header = "rank,operation,execution_time\n";
            MPI_CHECK(MPI_File_write(file, header, strlen(header), MPI_CHAR,
                                     MPI_STATUS_IGNORE),
                      true);
        }

        MPI_Barrier(info->comm);
    }

    // The data to be written.
    char buffer[128];
    int line_len = snprintf(buffer, sizeof(buffer), "%3d,%-12s,%f,%4d\n",
                            info->rank, op, *exec_time, round);
    
    // Calculate the offset to skip writing over the header.
    MPI_Offset header_offset = 30;
    MPI_Offset rank_offset = header_offset + *total_written +
                             (info->rank * line_len);

    MPI_CHECK(MPI_File_write_at(file, rank_offset, buffer, line_len, MPI_CHAR,
                                MPI_STATUS_IGNORE), true);

    if (total_written != NULL) {
        *total_written += info->size * line_len;
    }
    
    return 0;
}
