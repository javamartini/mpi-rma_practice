# MPI RMA Practice

A practice repository for learning one-way interprocess communication. I am
attempting to learn it from a higher level so when I attempt to enact it using
libfabric, I would hopefully have a better concept mentally.

## Usage

In it's current state, it shouldn't be ran by more than 2 processes. There are
no arguments. Just run it with `mpirun`.

```
mpirun -n 2 ./build/mpi-rma
```

## Building

The application is built using CMake, so that the application can be run across
BSD-forks and Linux distributions.

Obviously, the only dependence here is the installation of OpenMPI.

For FreeBSD...

```
pkg install openmpi
```

For Linux using the Apt package manager...

```
apt install openmpi-bin openmpi-common libopenmpi-dev
```

Then, just build the application with CMake.

```
cmake -S . -B build
cmake --build build
```

## Author

Javontae Alexander Martin
