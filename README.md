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

The application will want a place to write data to a file. It will first search
for environment variable `XDG_DATA_HOME`, but then search for `HOME` as a
fallback. Data will be written in the directory `mpi-rma_practice` if the
`XDG_DATA_HOME` variable was found. If the `HOME` variable was found, then the
data can be found at `~/.mpi-rma_practice`.

To visualize the data from the program, there are python scripts provided. It
is best to create a Python 3 virtual environment to isolate installed packages
for the scripts. The following assumes that you are at the project root
directory.

```
python3 -m venv .venv

# If using bash.
source .venv/bin/activate

# If use sh.
. .venv/bin/activate

python3 -m pip install -r pip_requirements.txt
```

Now you can run the Python scripts for data visualization without installing
packages system-wide. Isolation is peak.

**NOTE**: To leave the virtual environment, run `deactivate` or
`.venv/bin/deactivate`.

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
