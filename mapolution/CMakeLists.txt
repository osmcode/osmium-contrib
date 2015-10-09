#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
include(add_dev_configuration)
project(osmium-mapolution)

cmake_minimum_required(VERSION 2.8.5)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

find_package(Boost REQUIRED COMPONENTS program_options filesystem system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "../../libosmium")
endif()
find_package(Osmium REQUIRED COMPONENTS io gdal proj sparsehash)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

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
