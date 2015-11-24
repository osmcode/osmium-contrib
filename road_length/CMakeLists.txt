#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(add_dev_configuration)
project(osmium-road-length)

find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "../../libosmium")
endif()
find_package(Osmium REQUIRED COMPONENTS io sparsehash)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG road_length)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

add_test(road_length road_length ${CMAKE_CURRENT_SOURCE_DIR}/test/road.osm)

set_tests_properties(road_length PROPERTIES
                     PASS_REGULAR_EXPRESSION "^Length: 0\\.405.*km\n$"
)


#----------------------------------------------------------------------
