#ifndef IO_HPP
#define IO_HPP

// For inter-process communication.
#include "comm.hpp"
#include <mpi.h>

#include <print>
#include <string>
#include <cstdarg>

// Log to both the CLI and a log-file.
int log(MPI_File* file, WorldInfo* info, int type, std::string& format, ...);

// Write execution time data of each process to a file.
int write_execution_time(MPI_File* file, WorldInfo* info, std::string& op,
	double* exec_time);

#endif // IP_HPP
