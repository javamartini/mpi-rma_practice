cmake_minimum_required(VERSION 3.26)
project(mpi-rma_practice)

# === Development-related arguments. ===
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Required packages for this project. ===
find_package(MPI REQUIRED)

# === Specify the compiler. ===
set(CMAKE_CXX_COMPILER ${MPI_C_COMPILER})

set(PROJ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mpi-rma_practice)

# === Executable definition. ===
add_executable(mpi-rma
	${PROJ_DIR}/src/main.c
	${PROJ_DIR}/src/io.c
)

# === Included items to be compiled against 'mpi-rma'. ===
target_include_directories(mpi-rma
	PRIVATE ${PROJ_DIR}/include
	PRIVATE ${MPI_INCLUDE_PATH}
)

# === Libraries to be linked to target 'mpi-rma'. ===
target_link_libraries(mpi-rma
	PRIVATE ${MPI_LIBRARIES}
)
