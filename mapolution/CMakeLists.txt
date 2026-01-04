#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(add_dev_configuration)
project(osmium-mapolution)

find_package(Boost CONFIG REQUIRED COMPONENTS program_options)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "../../libosmium;../../protozero")
endif()
find_package(Osmium 2.13.1 REQUIRED COMPONENTS io gdal)
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
set_pthread_on_target(${PROG})

add_test(mapolution_help mapolution --help)

set_tests_properties(mapolution_help PROPERTIES
                     PASS_REGULAR_EXPRESSION "Usage: mapolution"
)


#----------------------------------------------------------------------
