# Compilation script for Genesis 1.3
cmake_minimum_required (VERSION 3.10)
project(Genesis13 LANGUAGES C CXX)

set(HDF5_PREFER_PARALLEL TRUE)
find_package(MPI REQUIRED COMPONENTS C CXX MPICXX)
find_package(HDF5 REQUIRED COMPONENTS C)

add_definitions(-DMPE -DH5_HAVE_PARALLEL
                      ${HDF5_DEFINITIONS}
                      ${MPI_C_COMPILE_DEFINITIONS}
                      ${MPI_CXX_COMPILE_DEFINITIONS}
                      ${MPI_MPICXX_COMPILE_DEFINITIONS})

include_directories(include
                    ${HDF5_INCLUDE_DIRS}
                    ${MPI_C_INCLUDE_DIRS}
                    ${MPI_CXX_INCLUDE_DIRS}
                    ${MPI_MPICXX_INCLUDE_DIRS})

file(GLOB genesis_sources src/libgenesis13/*/*.cpp)

add_library(genesis13 ${genesis_sources})
target_link_libraries(genesis13 ${HDF5_LIBRARIES} 
                                ${HDF5_HL_LIBRARIES}
                                ${MPI_CXX_LIBRARIES})


set_target_properties(genesis13 PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
set_target_properties(genesis13 PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")


add_executable(gencore src/gencore/mainwrap.cpp)
target_link_libraries(gencore PRIVATE genesis13 ${HDF5_LIBRARIES} 
                                                ${HDF5_HL_LIBRARIES}
                                                ${MPI_CXX_LIBRARIES})
set_target_properties(gencore PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
set_target_properties(gencore PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}") 
