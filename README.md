Genesis 1.3
===========

Description
-----------

Fork of svenreiche/Genesis-1.3-Version4. 
This repository aim is to add some enhancements to the original code and port it to C++11.
The enhancements comprises performances improvements, codebase size reduction through the use of standard library/Boost C++, User interface improvements and code refactoring.

Ideas for development
---------------------
| **Name** | **Description** | **Development** |
|----------|-----------------|---------------|
| CMake support | Use [CMake](https://cmake.org/) to configure the build of genesis | **Done** |
| Boost support | Add [Boost C++](https://www.boost.org/) support in build script | **Done** | 
| Remove Invertfc and Besselj code | Remove Invertfc and Besselj from code base and substitute them with functions in [boost::math](https://www.boost.org/doc/libs/1_69_0/libs/math/doc/html/special.html) | **To test** |
| General refactoring | General refactoring of interfaces, commenting, use of STL facilities ([like smart pointers](https://en.cppreference.com/book/intro/smart_pointers)) | **In progress** |
| CUDA support | Use [CUDA](https://developer.nvidia.com/) to speed up computations using NVIDIA GPUs | **Not started** |
| OpenMP support | Use [OpenMP](https://www.openmp.org/) to speed up computations when multicore nodes are available | **Not started** |
| Tests | Add support to tests. This can be done with [CTest](https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest) and [boost::test](https://www.boost.org/doc/libs/1_69_0/libs/test/doc/html/index.html) | **Not started** |
| TOML input files | Use [TOML](https://github.com/toml-lang/toml) for input and lattice files.<br> A possible library to use is [cpptoml](https://github.com/skystrife/cpptoml) | **Not started** |
| Spirit parser | Use [boost::spirit](https://www.boost.org/doc/libs/1_69_0/libs/spirit/doc/html/index.html) to parse .in and .lat files | **Not started** |
| Single configuration file format | Make it possible to define the configuration of a simulation in a single file | **Not started** |
| Command line arguments | Add command line parser to set verbosity/input-output file/other. A possible library to use is [boost::program_options](https://www.boost.org/doc/libs/1_69_0/doc/html/program_options.html) | **Not started** |
| Logging | Use [boost::logging](https://www.boost.org/doc/libs/1_69_0/libs/log/doc/html/index.html) for logging purpouses | **Not started** |
| Linear algebra | Use [boost::ublas](https://www.boost.org/doc/libs/1_69_0/libs/numeric/ublas/doc/index.html) algorithms and structures to perform linear algebra calculations.<br> If high performance is required, the inclusion of [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) in the codebase can be evaluated | **Not started** | 
| MPI | Use [boost::mpi](https://www.boost.org/doc/libs/1_69_0/doc/html/mpi.html) as MPI interface | **Not started** |
| Remove RandomU code | Remove RandomU code and substitute at least with [boost::random](https://www.boost.org/doc/libs/1_69_0/doc/html/boost_random.html) mt19937 engine | **Not started** |
| Fully reproducible simulations | Make it possible to make fully reproducible simulations idependently of the number of nodes/cores used. This needs the use of a CBPRNG (eg [librandom123](http://www.deshawresearch.com/resources_random123.html)) or skippable PRNG (eg [libpcg-cpp](http://www.pcg-random.org/)) | **Under evaluation** |
| Remove HDF5-parallel support | Change the output code generation to use the serial version of libhdf5. This would ease the configure and build process | **Under evaluation** |

Dependencies
------------

- openmpi-dev
- hdf5-openmpi-dev
- cmake
- boost

On debian-like systems these can be installed with:

```bash
    sudo apt-get install cmake openmpi-dev hdf5-openmpi-dev libbost-dev-all
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


