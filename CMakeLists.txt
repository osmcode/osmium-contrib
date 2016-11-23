#-----------------------------------------------------------------------------
#
#  CMake Config
#
#  Osmium-contrib
#
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


#-----------------------------------------------------------------------------
include(add_dev_configuration)
project(osmium-contrib)

enable_testing()

add_subdirectory(dense_tiles)
add_subdirectory(duplicate_nodes)
add_subdirectory(export_to_wkt)
add_subdirectory(mapolution)
add_subdirectory(node_density)

#-----------------------------------------------------------------------------
include(CppcheckTarget)
add_cppcheck_target(*/*.*pp)

#-----------------------------------------------------------------------------
