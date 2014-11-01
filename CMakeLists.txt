#----------------------------------------------------------------------
#
#  Osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
project(osmium-contrib)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(OsmiumOptions)

find_package(Boost REQUIRED COMPONENTS program_options system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io gdal proj sparsehash)
include_directories(${OSMIUM_INCLUDE_DIRS})

include(CppcheckTarget)
add_cppcheck_target(**/*.cpp)

macro(COMPILE_PROGRAM PROG)
  file(GLOB SOURCES ${PROG}/*.cpp ${PROG}/*.hpp)
  add_executable(${PROG} ${SOURCES})
  target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})
endmacro()

COMPILE_PROGRAM(amenity_list)
COMPILE_PROGRAM(pub_names)
COMPILE_PROGRAM(road_length)
COMPILE_PROGRAM(export_to_wkt)
COMPILE_PROGRAM(node_density)
COMPILE_PROGRAM(mapolution)
