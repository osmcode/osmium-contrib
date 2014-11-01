#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
project(osmium-node-density)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

include(OsmiumOptions)

find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io gdal proj sparsehash)
include_directories(${OSMIUM_INCLUDE_DIRS})

include(CppcheckTarget)
add_cppcheck_target(*.*pp)

set(PROG node_density)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})
