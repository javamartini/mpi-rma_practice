#include <mpi.h>

#include <vector>
#include <algorithm>
#include <print>
#include <cstdlib>

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);

	// Get the rank of the process and the total number of processes.
	int rank = -1, size = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Split processes 0 and 1 into their own communicators.
	MPI_Comm comm;
	MPI_Comm_split(MPI_COMM_WORLD, rank <= 1, rank, &comm);

	// Make a message to make availble via a window.
	std::vector<float> buf(10);
	if (rank == 0)
		std::fill(buf.begin(), buf.end(), 32.4);

	std::print("rank: {}, buffer: ", rank);
	for (auto i : buf)
		std::print("{} ", i);
	std::print("\n");

	// Create a window for both processes based on the buffer being put.
	MPI_Win win;
	if (rank == 0)
		MPI_Win_create(MPI_BOTTOM, 0, sizeof(float), MPI_INFO_NULL, comm, &win);
	else
		/* Expose the memory we want to put data into, and specify the current
		 * byte size of the buffer. */
		MPI_Win_create(buf.data(), buf.size() * sizeof(float), sizeof(float),
				MPI_INFO_NULL, comm, &win);

	// Put ten floats from rank 0's buffer into rank 1's window. 
	MPI_Win_fence(0, win);
	if (rank == 0)
		MPI_Put(buf.data(), buf.size(), MPI_FLOAT, 1, 0,
				buf.size(), MPI_FLOAT, win);

	// Complete the 'Put' operation.
	MPI_Win_fence(0, win);

	std::print("rank: {}, buffer: ", rank);
	for (auto i : buf)
		std::print("{} ", i);
	std::print("\n");

	// Free resources used for this operation: window, custom communicator.
	MPI_Win_free(&win);
	MPI_Comm_free(&comm);

	MPI_Finalize();
	return EXIT_SUCCESS;
}
