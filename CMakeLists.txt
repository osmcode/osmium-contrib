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
project(osmium-contrib)

include(common)

find_package(Boost REQUIRED COMPONENTS program_options system filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io gdal proj sparsehash)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})


#-----------------------------------------------------------------------------
#
#  Configure all subprojects on their own
#
#-----------------------------------------------------------------------------
macro(configure_subproject _subproject)
    file(GLOB SOURCES ${_subproject}/*.cpp ${_subproject}/*.hpp)
    add_executable(${_subproject} ${SOURCES})
    target_link_libraries(${_subproject} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})
endmacro()

configure_subproject(amenity_list)
configure_subproject(export_to_wkt)
configure_subproject(mapolution)
configure_subproject(node_density)
configure_subproject(pub_names)
configure_subproject(road_length)


#-----------------------------------------------------------------------------
