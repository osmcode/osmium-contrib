#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
project(osmium-road-length)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io sparsehash)
include_directories(${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG road_length)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

add_test(NAME road_length
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND road_length ${CMAKE_CURRENT_SOURCE_DIR}/test/road.osm
)
set_tests_properties(road_length PROPERTIES
                     PASS_REGULAR_EXPRESSION "^Length: 0\\.405.*km\n$"
)


#----------------------------------------------------------------------
