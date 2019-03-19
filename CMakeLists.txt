# Compilation script for Genesis 1.3
cmake_minimum_required (VERSION 3.13)

set(VERSIONMAJOR 4)
set(VERSIONMINOR 3)
set(VERSIONREVISION 0)
set(VERSIONBETA true)

project(Genesis13 
    VERSION "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONREVISION}"
    DESCRIPTION "Genesis 1.3 V4. Tool to simulate beam-field interaction \
    in Free Electron Lasers"
    HOMEPAGE_URL "https://github.com/bellaz89/Genesis-1.3-Version4"
    LANGUAGES C CXX)

add_compile_definitions(
    VERSIONMAJOR=${VERSIONMAJOR}
    VERSIONMINOR=${VERSIONMINOR}
    VERSIONREVISION=${VERSIONREVISION}
    VERSIONBETA=${VERSIONBETA})

set (CMAKE_CXX_STANDARD 11)
include(CheckLanguage)
find_package(PkgConfig REQUIRED)

set(default_build_type "Debug")

# openmpi and hdf5 libraries
pkg_check_modules(MPI REQUIRED ompi-cxx)
pkg_check_modules(HDF5 REQUIRED hdf5-openmpi)

file(GLOB COMMON_SOURCES src/libgenesis13/*/*.cpp)
set(COMMON_INCLUDE_DIRS src/)
set(PROJECT_INCLUDE_DIRS ${MPI_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
list(APPEND COMMON_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS})
set(COMMON_CFLAGS ${MPI_CFLAGS} ${HDF5_CFLAGS})
set(COMMON_LIB_DIRS ${MPI_LIBRARY_DIRS} ${HDF5_LIBRARY_DIRS})
set(COMMON_LIBRARIES ${MPI_LIBRARIES} ${HDF5_LIBRARIES})
set(COMMON_LDFLAGS ${MPI_LDFLAGS} ${HDF5_LDFLAGS})

#------------------------------------------------------------------------------
# Genesis 1.3 C++ build rules
#------------------------------------------------------------------------------

file(GLOB CPP_SOURCES src/libgenesis13/*/cpp/*.cpp)

add_library(genesis13 ${COMMON_SOURCES} ${CPP_SOURCES})
target_include_directories(genesis13 PUBLIC include PRIVATE ${COMMON_INCLUDE_DIRS}) 
target_compile_definitions(genesis13 PUBLIC -DMPE)
target_compile_options(genesis13 PUBLIC ${COMMON_CFLAGS})
target_link_libraries(genesis13 PRIVATE ${COMMON_LIBRARIES}) 
target_link_options(genesis13 PUBLIC ${COMMON_LDFLAGS})

add_executable(gencore src/gencore/mainwrap.cpp)
target_include_directories(gencore  PRIVATE include COMMON_INCLUDE_DIRS)
target_compile_definitions(gencore PRIVATE -DMPE)
target_compile_options(gencore PRIVATE ${COMMON_CFLAGS})
target_link_directories(gencore PRIVATE ${COMMON_LIB_DIRS})
target_link_libraries(gencore PRIVATE genesis13)
target_link_options(gencore PRIVATE ${COMMON_LDFLAGS})

install(TARGETS genesis13 gencore)
#------------------------------------------------------------------------------
# Genesis 1.3 CUDA build rules
#------------------------------------------------------------------------------

check_language(CUDA)
if(FALSE)

    enable_language(CUDA)

    file(GLOB CUDA_SOURCES src/libgenesis13/*/cuda/*.cpp src/libgenesis13/*/cuda/*.cu)

    add_library(genesis13-cuda ${COMMON_SOURCES} ${CUDA_SOURCES})
    set_target_properties(genesis13-cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    target_include_directories(genesis13-cuda PUBLIC include PRIVATE ${COMMON_INCLUDE_DIRS}) 
    target_compile_definitions(genesis13-cuda PUBLIC -DMPE)

    if("-pthread" IN_LIST COMMON_CFLAGS) 
        # Delete -pthread because it is unsupported by nvcc

        list(REMOVE_ITEM COMMON_CFLAGS -pthread)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler -pthread")

    endif()

    target_compile_options(genesis13-cuda PUBLIC ${COMMON_CFLAGS})
    target_link_libraries(genesis13-cuda PRIVATE ${COMMON_LIBRARIES}) 
    target_link_options(genesis13-cuda PUBLIC ${COMMON_LDFLAGS})

    add_executable(gencore-cuda src/gencore/mainwrap.cpp)
    target_include_directories(gencore-cuda  PRIVATE include ${COMMON_INCLUDE_DIRS})
    target_compile_definitions(gencore-cuda PRIVATE -DMPE)
    target_compile_options(gencore-cuda PRIVATE ${COMMON_CFLAGS})
    target_link_directories(gencore-cuda PRIVATE ${COMMON_LIB_DIRS})
    target_link_libraries(gencore-cuda PRIVATE genesis13-cuda)
    target_link_options(gencore-cuda PRIVATE ${COMMON_LDFLAGS})

    install(TARGETS genesis13-cuda gencore-cuda)
endif()    

