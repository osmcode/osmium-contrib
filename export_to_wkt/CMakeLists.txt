#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
project(osmium-amenity-list)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io sparsehash)
include_directories(${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG export_to_wkt)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

add_test(NAME export_to_wkt
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND export_to_wkt ${CMAKE_CURRENT_SOURCE_DIR}/test/node.osm
)
set_tests_properties(export_to_wkt PROPERTIES
                     PASS_REGULAR_EXPRESSION "n24960505 POINT\\(8\\.8720536 53\\.096629\\)\n"
)


#----------------------------------------------------------------------
