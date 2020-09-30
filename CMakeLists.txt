cmake_minimum_required(VERSION 3.0)

project(main)

find_package(MPI REQUIRED)

add_executable(main compute_integrals.cpp)
target_link_libraries(main
  PRIVATE
  MPI_C)
