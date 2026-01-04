#-----------------------------------------------------------------------------
#
#  CMake Config
#
#  Osmium-contrib
#
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-----------------------------------------------------------------------------
include(add_dev_configuration)
project(osmium-contrib)

enable_testing()

add_subdirectory(dense_tiles)
add_subdirectory(export_to_wkt)
add_subdirectory(mapolution)
add_subdirectory(node_density)

#-----------------------------------------------------------------------------
include(CppcheckTarget)
add_cppcheck_target(*/*.*pp)

#-----------------------------------------------------------------------------
