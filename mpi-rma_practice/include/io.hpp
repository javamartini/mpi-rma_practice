#ifndef IO_HPP
#define IO_HPP

// For inter-process communication.
#include "comm.hpp"
#include <mpi.h>

#include <filesystem>
#include <print>
#include <string>

namespace filesys = std::filesystem;

// Ensure the data directory is present for data storage.
int ensure_data_dir(WorldInfo* info, filesys::path& file_path);

// Write execution time data of each process to a file.
int write_execution_time(WorldInfo* info, MPI_File* file, std::string& op,
	double* exec_time);

#endif // IP_HPP
