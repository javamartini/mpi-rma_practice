// Inter-process commmunication.
#include "comm.hpp"
#include <mpi.h>

// I/O-related headers and libraries.
#include "io.hpp"
#include <filesystem>

#include <string>
#include <vector>
#include <algorithm>
#include <print>
#include <cstdlib>

int main(int argc, char *argv[]) {
  WorldInfo info;
  MPI_Status status;
  std::string data_file_path_str =
      "/home/javontae/.local/state/mpi-rma_practice/exec_times.csv";
  int rc = -1;

  MPI_Init(&argc, &argv);

  // Capture the current process rank and the total number of processes.
  MPI_Comm_rank(MPI_COMM_WORLD, &info.rank);
  MPI_Comm_size(MPI_COMM_WORLD, &info.size);

  filesys::path data_file_path(data_file_path_str);
  if (info.rank == 0)
    rc = ensure_data_dir(&info, data_file_path);

  MPI_Bcast(&rc, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rc != 0) {
    std::print(stderr,
               "Rank {} aborting due to exit code of ensure_data_dir(): {}\n",
               info.rank, rc);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  /* Split processes 0 and 1 into their own communicators for one-way
   * communication. */
  MPI_Comm comm;
  MPI_Comm_split(MPI_COMM_WORLD, info.rank <= 1, info.rank, &comm);

  // Make a message to make availble via a window.
  std::vector<float> buf(10);
  if (info.rank == 0)
    std::fill(buf.begin(), buf.end(), 32.4);

  std::print("rank: {}, buffer: ", info.rank);
  for (auto i : buf)
    std::print("{} ", i);
  std::print("\n");

  // Create a window for both processes based on the buffer being put.
  MPI_Win win;
  if (info.rank == 0) {
    MPI_Win_create(MPI_BOTTOM, 0, sizeof(float), MPI_INFO_NULL, comm, &win);
  } else {
    /* Expose the memory we want to put data into, and specify the current
     * byte size of the buffer. */
    MPI_Win_create(buf.data(), buf.size() * sizeof(float), sizeof(float),
                   MPI_INFO_NULL, comm, &win);
  }

  // Start timing.
  const double start_time = MPI_Wtime();

  // Put ten floats from rank 0's buffer into rank 1's window.
  MPI_Win_fence(0, win);
  if (info.rank == 0)
    MPI_Put(buf.data(), buf.size(), MPI_FLOAT, 1, 0, buf.size(), MPI_FLOAT,
            win);

  // Complete the 'Put' operation.
  MPI_Win_fence(0, win);

  // Stop timing and calculate the execution time.
  const double end_time = MPI_Wtime();
  const double execution_time = end_time - start_time;
  std::print("{}\n", execution_time);

  std::print("rank: {}, buffer: ", info.rank);
  for (auto i : buf)
    std::print("{} ", i);
  std::print("\n");

  MPI_Barrier(MPI_COMM_WORLD);

  // Modify MPI settings to force filesystem driver.
  MPI_Info ih;
  MPI_Info_set(ih, "io_library", "romio_posix");

  // Open a file for storing collected data.
  MPI_File fh;
  rc = MPI_File_open(MPI_COMM_WORLD,
                     data_file_path_str.c_str(),
                     MPI_MODE_CREATE | MPI_MODE_RDWR, ih, &fh);
  if (rc != MPI_SUCCESS) {
    char err_str[MPI_MAX_ERROR_STRING];
    int err_len = -1;
    MPI_Error_string(rc, err_str, &err_len);
    std::print(stderr, "MPI_File_open(): {}\n", err_str);

    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Free gathered resources.
  MPI_Info_free(&ih);
  MPI_File_close(&fh);
  MPI_Win_free(&win);
  MPI_Comm_free(&comm);
  MPI_Finalize();

  return EXIT_SUCCESS;
}
