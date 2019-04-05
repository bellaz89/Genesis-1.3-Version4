Genesis 1.3
===========

Description
-----------

Fork of svenreiche/Genesis-1.3-Version4. 
This repository aim is to add some enhancements to the original code and port it to C++11.
The enhancements comprises performances improvements, codebase size reduction through the use of standard C++11 library/Boost, User interface improvements and code refactoring.

Ideas for development
---------------------
| **Name** | **Description** | **Development** |
|----------|-----------------|---------------|
| CMake support | Use [CMake](https://cmake.org/) to configure the build of genesis | **Done** |
| Boost support | Add [Boost C++](https://www.boost.org/) support in build script | **Done** | 
| Remove Invertfc and Besselj code | Remove Invertfc and Besselj from code base and substitute them with functions in [boost::math](https://www.boost.org/doc/libs/1_69_0/libs/math/doc/html/special.html) | **Done** |
| General refactoring | General refactoring of interfaces, commenting, use of STL facilities ([like smart pointers](https://en.cppreference.com/book/intro/smart_pointers)) | **In progress** |
| OpenMP support | Use [OpenMP](https://www.openmp.org/) to speed up computations when multicore nodes are available | **Not started** |
| Tests | Add support to tests. This can be done with [CTest](https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest) and [GTest](https://github.com/google/googletest) | **Done** |
| TOML input files | Use [TOML](https://github.com/toml-lang/toml) for input and lattice files. A possible library to use is [cpptoml](https://github.com/skystrife/cpptoml) | **Not started** |
| Spirit parser | Use [boost::spirit](https://www.boost.org/doc/libs/1_69_0/libs/spirit/doc/html/index.html) to parse .in and .lat files | **Not started** |
| Single configuration file format | Make it possible to define the configuration of a simulation in a single file | **Not started** |
| Command line arguments | Add command line parser to set verbosity/input-output file/other. A possible library to use is [boost::program_options](https://www.boost.org/doc/libs/1_69_0/doc/html/program_options.html) | **Not started** |
| Logging | Use [boost::logging](https://www.boost.org/doc/libs/1_69_0/libs/log/doc/html/index.html) for logging purpouses | **Not started** |
| Linear algebra | Use [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) algorithms and structures to perform linear algebra calculations. | **In progress** | 
| Remove RandomU code | Remove RandomU code and substitute at least with [boost::random](https://www.boost.org/doc/libs/1_69_0/doc/html/boost_random/reference.html#boost_random.reference.generators) mt19937 engine | **Not started** |
| Improved PRNG | Test the if a CBPRNG (eg [librandom123](http://www.deshawresearch.com/resources_random123.html)) or skippable PRNG (eg [libpcg-cpp](http://www.pcg-random.org/)) can improve code performance/randomness of the generated distributions | **Under evaluation** |
| Evaluate other QRNG | See if other Quasi Random Number Generator in [boost::random](https://www.boost.org/doc/libs/1_69_0/doc/html/boost_random/reference.html#boost_random.reference.concepts.quasi_random_number_generator) can substitute the original Hammerslay generator. Options are niederreiter, sobol, faure| **Not started** |
| Improved PRNG | Test the if a CBPRNG (eg [librandom123](http://www.deshawresearch.com/resources_random123.html)) or skippable PRNG (eg [libpcg-cpp](http://www.pcg-random.org/)) can improve code performance/randomness of the generated distributions | **Under evaluation** |
| CUDA support | Use [CUDA](https://developer.nvidia.com/) to speed up computations using NVIDIA GPUs | **Under evaluation** |

Dependencies
------------

- openmpi-dev
- HDF55-openmpi
- CMake >= 3.10
- Boost >= 1.65
- Eigen >= 3.3
- GoogleTest

On debian-like systems these can be installed with:

```bash
    sudo apt-get install build-essential cmake openmpi-dev hdf5-openmpi-dev libbost-dev-all libgtest-dev libeigen3-dev
```

Note that to build the code a compiler that supports C++11 is required.

Build instruction
-----------------

For the standard C++ version:

```bash
    mkdir build
    cd build
    cmake ..
    make gencore
```


To run tests (after cmake is run):

```bash
    make test
```
