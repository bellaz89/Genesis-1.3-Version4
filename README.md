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

Ideas
-----

- Use [TOML](https://github.com/toml-lang/toml) or [YAML](https://yaml.org/) 
  for input and lattice files?
  A standard filetype will make it easier
  to interoperate the program with external program and programming language.
  An additional benefit of use an external library 
  (ex [cpptoml](https://github.com/skystrife/cpptoml)) will make the codebase
  easier to mantain. Also it could be possible to merge .in and .lat files in a 
  single one.
