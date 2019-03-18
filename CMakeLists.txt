# Compilation script for Genesis 1.3
cmake_minimum_required (VERSION 3.13)
project(Genesis13 LANGUAGES C CXX)
include(CheckLanguage)
find_package(PkgConfig REQUIRED)

# openmpi and hdf5 libraries
pkg_check_modules(MPI REQUIRED ompi-cxx)
pkg_check_modules(HDF5 REQUIRED hdf5-openmpi)

file(GLOB genesis_sources src/libgenesis13/*/*.cpp)

#------------------------------------------------------------------------------
# Genesis 1.3 C++ build rules
#------------------------------------------------------------------------------

file(GLOB genesis_CPP_sources src/libgenesis13/*/cpp/*.cpp)

add_library(genesis13 ${genesis_sources} ${genesis_CPP_sources})
target_include_directories(genesis13  PUBLIC include 
    PRIVATE ${MPI_INCLUDE_DIRS}
    PRIVATE ${HDF5_INCLUDE_DIRS})

target_compile_definitions(genesis13 PUBLIC -DMPE)
target_compile_options(genesis13 PUBLIC ${MPI_CFLAGS} ${HDF5_CFLAGS})
target_link_options(genesis13 PUBLIC ${MPI_LDFLAGS} ${HDF5_LDFLAGS})

add_executable(gencore src/gencore/mainwrap.cpp)
target_include_directories(gencore  PRIVATE include 
    PRIVATE ${MPI_INCLUDE_DIRS}
    PRIVATE ${HDF5_INCLUDE_DIRS})

target_compile_definitions(gencore PRIVATE -DMPE)
target_compile_options(gencore PRIVATE ${MPI_CFLAGS} ${HDF5_CFLAGS})
target_link_directories(gencore PRIVATE ${MPI_LIBRARY_DIRS} ${HDF5_LIBRARY_DIRS})
target_link_libraries(gencore PRIVATE genesis13 ${MPI_LIBRARIES} ${HDF5_LIBRARIES}) 
target_link_options(gencore PRIVATE ${MPI_LDFLAGS} ${HDF5_LDFLAGS})

#------------------------------------------------------------------------------
# Genesis 1.3 CUDA build rules
#------------------------------------------------------------------------------

check_language(CUDA)
if(CMAKE_CUDA_COMPILER)

    enable_language(CUDA)

    file(GLOB genesis_CUDA_sources_cpp src/libgenesis13/*/cuda/*.cpp)
    file(GLOB genesis_CUDA_sources_cu src/libgenesis13/*/cuda/*.cu)

    add_library(genesis13-cuda ${genesis_sources} 
                               ${genesis_CUDA_sources_cpp}
                               ${genesis_CUDA_sources_cu})
    target_include_directories(genesis13-cuda  PUBLIC include 
        PRIVATE ${MPI_INCLUDE_DIRS}
        PRIVATE ${HDF5_INCLUDE_DIRS})

    target_compile_definitions(genesis13-cuda PUBLIC -DMPE -DUSE_CUDA)
    target_compile_options(genesis13-cuda PUBLIC ${MPI_CFLAGS} ${HDF5_CFLAGS})
    target_link_options(genesis13-cuda PUBLIC ${MPI_LDFLAGS} ${HDF5_LDFLAGS})

    add_executable(gencore-cuda src/gencore/mainwrap.cpp)
    target_include_directories(gencore-cuda  PRIVATE include 
        PRIVATE ${MPI_INCLUDE_DIRS}
        PRIVATE ${HDF5_INCLUDE_DIRS})

    target_compile_definitions(gencore-cuda PRIVATE -DMPE -DUSE_CUDA)
    target_compile_options(gencore-cuda PRIVATE ${MPI_CFLAGS} ${HDF5_CFLAGS})
    target_link_directories(gencore-cuda PRIVATE ${MPI_LIBRARY_DIRS} ${HDF5_LIBRARY_DIRS})
    target_link_libraries(gencore-cuda PRIVATE genesis13-cuda ${MPI_LIBRARIES} ${HDF5_LIBRARIES}) 
    target_link_options(gencore-cuda PRIVATE ${MPI_LDFLAGS} ${HDF5_LDFLAGS})
    install(TARGETS genesis13-cuda gencore-cuda)

endif()    

install(TARGETS genesis13 gencore)
