# Compilation script for Genesis 1.3
cmake_minimum_required (VERSION 3.13)

project(Genesis13 
    VERSION 4.3.0
    DESCRIPTION "Genesis 1.3 V4. Tool to simulate beam-field interaction \
    in Free Electron Lasers"
    HOMEPAGE_URL "https://github.com/bellaz89/Genesis-1.3-Version4"
    LANGUAGES C CXX)

# pass the project version as compile time preprocessor flags
add_compile_definitions(
    VERSIONMAJOR=${PROJECT_VERSION_MAJOR}
    VERSIONMINOR=${PROJECT_VERSION_MINOR}
    VERSIONREVISION=${PROJECT_VERSION_PATCH}
    VERSIONBETA=true)

set (CMAKE_CXX_STANDARD 11)
include(CheckLanguage)
find_package(PkgConfig REQUIRED)

set(default_build_type "Release")

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
target_link_libraries(gencore PUBLIC genesis13)
target_link_options(gencore PRIVATE ${COMMON_LDFLAGS})

install(TARGETS gencore DESTINATION bin)
install(TARGETS genesis13 LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

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
    target_link_libraries(gencore-cuda PUBLIC genesis13-cuda)
    target_link_options(gencore-cuda PRIVATE ${COMMON_LDFLAGS})

    install(TARGETS gencore-cuda DESTINATION bin)
    install(TARGETS genesis13-cuda LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
endif()    

# Packaging support
set(CPACK_PACKAGE_NAME "genesis13v4")
set(CPACK_PACKAGE_VENDOR "Andrea Bellandi")
set(CPACK_PACKAGE_CONTACT "bellaz89@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FEL simulator")
set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
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

set(CPACK_BINARY_RPM TRUE)
set(CPACK_RPM_PACKAGE_GROUP "General")
set(CPACK_RPM_PACKAGE_DEPENDS "openmpi-devel, hdf5-openmpi-devel, cmake (>= 3.13), gcc, gcc-c++")

set(CPACK_BINARY_DEBIAN TRUE)
set(CPACK_DEBIAN_PACKAGE_GROUP "General")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libopenmpi-dev, libhdf5-openmpi-dev, cmake (>= 3.13), build-essential")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Andrea Bellandi <bellaz89@gmail.com>")

include(CPack)
