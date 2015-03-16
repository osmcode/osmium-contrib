#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
project(osmium-mapolution)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

find_package(Boost REQUIRED COMPONENTS program_options filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Osmium REQUIRED COMPONENTS io gdal proj sparsehash)
include_directories(${OSMIUM_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

if(HANDLER)
    add_definitions(-DHANDLER=${HANDLER})
    message(STATUS "Using handler: ${HANDLER}")
endif()

#----------------------------------------------------------------------

set(PROG mapolution)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})

add_test(NAME mapolution_help
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND mapolution --help
)
set_tests_properties(mapolution_help PROPERTIES
                     PASS_REGULAR_EXPRESSION "Usage: mapolution"
)


#----------------------------------------------------------------------
