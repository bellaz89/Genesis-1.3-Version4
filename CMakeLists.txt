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

set (CMAKE_CXX_STANDARD 11)
include(CheckLanguage)
find_package(PkgConfig REQUIRED)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
    endif()
endif()

# openmpi, boost, hdf5, eigen libraries
set(BUILD_MULTITHREADED OFF)
set(Boost_USE_MULTITHREADED ${BUILD_MULTITHREADED})
set(HDF5_PREFER_PARALLEL TRUE)
find_package(Boost 1.65 REQUIRED COMPONENTS random mpi unit_test_framework)
find_package(MPI REQUIRED COMPONENTS CXX)
find_package(HDF5 REQUIRED COMPONENTS C)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

file(GLOB COMMON_SOURCES src/libgenesis13/*/*.cpp)
set(COMMON_INCLUDE_DIRS src/)
set(PROJECT_INCLUDE_DIRS ${MPI_CXX_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
list(APPEND COMMON_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS})
set(COMMON_CFLAGS ${MPI_CXX_COMPILE_OPTIONS})
set(COMMON_LIB_DIRS ${Boost_INCLUDE_DIRS})
set(COMMON_LIBRARIES ${MPI_CXX_LIBRARIES} ${HDF5_LIBRARIES} ${Boost_LIBRARIES})
set(COMMON_LDFLAGS ${MPI_CXX_LINK_FLAGS})
add_definitions(${HDF5_DEFINITIONS})
link_directories(${COMMON_LIB_DIRS})

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
target_include_directories(genesis13 PUBLIC include/libgenesis13 PRIVATE ${COMMON_INCLUDE_DIRS}) 
target_compile_definitions(genesis13 PUBLIC -DMPE)
target_compile_options(genesis13 PUBLIC ${COMMON_CFLAGS} -Wall -Wextra)
target_link_libraries(genesis13 PRIVATE ${COMMON_LIBRARIES} Eigen3::Eigen) 

add_executable(gencore src/gencore/mainwrap.cpp)
target_include_directories(gencore  PRIVATE include/libgenesis13 PRIVATE ${COMMON_INCLUDE_DIRS})
target_compile_definitions(gencore PRIVATE -DMPE)
target_compile_options(gencore PRIVATE ${COMMON_CFLAGS} -Wall -Wextra)
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
