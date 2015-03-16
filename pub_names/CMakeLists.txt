#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
project(osmium-pub-names)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io sparsehash)
include_directories(${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG pub_names)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

add_test(NAME pub_names
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND pub_names ${CMAKE_CURRENT_SOURCE_DIR}/test/pubs.osm
)
set_tests_properties(pub_names PROPERTIES
                     PASS_REGULAR_EXPRESSION "^Im Holze\n$"
)


#----------------------------------------------------------------------
