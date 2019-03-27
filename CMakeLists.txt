# Compilation script for Genesis 1.3
cmake_minimum_required(VERSION 3.3)

project(Genesis13 
    VERSION 4.3.0
    LANGUAGES C CXX)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# pass the project version as compile time preprocessor flags
add_definitions(
    -DVERSIONMAJOR=${PROJECT_VERSION_MAJOR}
    -DVERSIONMINOR=${PROJECT_VERSION_MINOR}
    -DVERSIONREVISION=${PROJECT_VERSION_PATCH}
    -DVERSIONBETA=true)

set (CMAKE_CXX_STANDARD 17)
include(CheckLanguage)
find_package(PkgConfig REQUIRED)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif()
endif()

# openmpi, boost and hdf5 libraries
set(BUILD_MULTITHREADED OFF)
set(Boost_USE_MULTITHREADED ${BUILD_MULTITHREADED})

find_package(Boost REQUIRED COMPONENTS random mpi unit_test_framework)
pkg_check_modules(MPI REQUIRED ompi-cxx)
pkg_check_modules(HDF5 REQUIRED hdf5-openmpi)

file(GLOB COMMON_SOURCES src/libgenesis13/*/*.cpp)
set(COMMON_INCLUDE_DIRS src/)
set(PROJECT_INCLUDE_DIRS ${MPI_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
list(APPEND COMMON_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS})
set(COMMON_CFLAGS ${MPI_CFLAGS} ${HDF5_CFLAGS})
set(COMMON_LIB_DIRS ${MPI_LIBRARY_DIRS} ${HDF5_LIBRARY_DIRS} ${Boost_INCLUDE_DIRS})
set(COMMON_LIBRARIES ${MPI_LIBRARIES} ${HDF5_LIBRARIES} ${Boost_LIBRARIES})
set(COMMON_LDFLAGS ${MPI_LDFLAGS} ${HDF5_LDFLAGS})
link_directories(${COMMON_LIB_DIRS})

#------------------------------------------------------------------------------
# CUDA support
#------------------------------------------------------------------------------

if(ENABLE_CUDA)
    if(${CMAKE_VERSION} VERSION_LESS 3.10)
        message(ERROR "CUDA builds require CMAKE >= 3.10")
    endif()

    enable_language(CUDA)

    if(${CMAKE_CUDA_COMPILER_VERSION} VERSION_LESS 9.0)
        message(ERROR "CUDA requires nvcc version >= 9.0")
    endif()

    file(GLOB CUDA_SOURCES src/libgenesis13/*/cuda/*.cpp src/libgenesis13/*/cuda/*.cu)
    set(COMMON_SOURCES ${COMMON_SOURCES} ${CUDA_SOURCES}) 
    add_definitions(-DUSE_CUDA)

    if("-pthread" IN_LIST COMMON_CFLAGS) 
        # Delete -pthread because it is unsupported by nvcc
        list(REMOVE_ITEM COMMON_CFLAGS -pthread)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler -pthread")
    endif()
endif()

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------

find_package(GTest)
if(GTest_FOUND)
    enable_testing()
    file(GLOB TEST_SOURCES test/*.cpp)
    foreach(testsource ${TEST_SOURCES})
        get_filename_component(testname ${testsource} NAME_WE)
        add_executable("${testname}.test" ${testsource})
        target_link_libraries ("${testname}.test" genesis13 GTest::Main GTest::GTest)
        gtest_add_tests(TARGET "${testname}.test")
    endforeach(testsource)
endif()
#------------------------------------------------------------------------------
# Genesis support
#------------------------------------------------------------------------------

add_library(genesis13 ${COMMON_SOURCES})
if(ENABLE_CUDA)
    set_target_properties(genesis13 PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()
target_include_directories(genesis13 PUBLIC include/libgenesis13 PRIVATE ${COMMON_INCLUDE_DIRS}) 
target_compile_definitions(genesis13 PUBLIC -DMPE)
target_compile_options(genesis13 PUBLIC ${COMMON_CFLAGS})
target_link_libraries(genesis13 PRIVATE ${COMMON_LIBRARIES}) 

add_executable(gencore src/gencore/mainwrap.cpp)
target_include_directories(gencore  PRIVATE include/libgenesis13 COMMON_INCLUDE_DIRS)
target_compile_definitions(gencore PRIVATE -DMPE)
target_compile_options(gencore PRIVATE ${COMMON_CFLAGS})
target_link_libraries(gencore PUBLIC genesis13)

install(TARGETS gencore DESTINATION bin)
install(TARGETS genesis13 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)

#------------------------------------------------------------------------------
# Packaging support
#------------------------------------------------------------------------------

set(CPACK_PACKAGE_NAME "genesis13v4")
set(CPACK_PACKAGE_VENDOR "Andrea Bellandi")
set(CPACK_PACKAGE_CONTACT "bellaz89@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FEL simulator")
set(CPACK_PACKAGE_DESCRIPTION "Genesis 1.3 V4. Tool to simulate beam-field interaction \
in Free Electron Lasers")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_INSTALL_PREFIX "/usr")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.*build.*
    /\\\\.DS_Store
    )

set(CPACK_SOURCE_GENERATOR "TGZ;RPM;DEB")

set(CPACK_RPM_PACKAGE_GROUP "General")
set(CPACK_RPM_PACKAGE_DEPENDS "openmpi-devel, hdf5-openmpi-devel, cmake (>= 3.13), gcc, gcc-c++")

set(CPACK_DEBIAN_PACKAGE_GROUP "General")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libopenmpi-dev, libhdf5-openmpi-dev, cmake (>= 3.13), build-essential")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Andrea Bellandi <bellaz89@gmail.com>")

include(CPack)
