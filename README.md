Genesis 1.3
===========

Description
-----------

Fork of svenreiche/Genesis-1.3-Version4


Objectives
----------

- add CMake support (done)
- add CUDA support

Dependencies
------------

- openmpi-dev
- hdf5-openmpi-dev
- cmake

On debian-like systems these can be installed with:

```bash
    sudo apt-get install cmake openmpi-dev hdf5-openmpi-dev
```

Build instruction
-----------------

For the standard C++ version:

```bash
    mkdir build
    cd build
    cmake ..
    make gencore
```


