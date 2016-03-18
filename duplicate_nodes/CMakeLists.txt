#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(add_dev_configuration)
project(osmium-duplicate-nodes)

find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "../../libosmium")
endif()
find_package(Osmium REQUIRED COMPONENTS io)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG duplicate_nodes)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${OSMIUM_LIBRARIES})


#----------------------------------------------------------------------
