#include "io.h"
#include "comm.h"

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
    rc = mkdir(data_dir_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (rc != 0) {
      perror("mkdir()");
      return 1;
    }
  }

  return 0;
}
