cmake_minimum_required(VERSION 3.31)
project(mpi-rma_practice CXX)

# === Development-related arguments. ===
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Required packages for this project. ===
find_package(MPI CXX REQUIRED)
