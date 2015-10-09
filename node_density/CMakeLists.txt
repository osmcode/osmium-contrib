#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
include(add_dev_configuration)
project(osmium-node-density)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "../../libosmium")
endif()
find_package(Osmium REQUIRED COMPONENTS io gdal proj sparsehash)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG node_density)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

add_test(NAME node_density_help
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND node_density --help
)
set_tests_properties(node_density_help PROPERTIES
                     PASS_REGULAR_EXPRESSION "Usage: node_density"
)


#----------------------------------------------------------------------
