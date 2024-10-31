#----------------------------------------------------------------------
#
#  Single example osmium-contrib CMakeLists.txt
#
#----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(add_dev_configuration)
project(osmium-export-to-wkt)

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "../../libosmium;../../protozero")
endif()
find_package(Osmium 2.13.1 REQUIRED COMPONENTS io)
include_directories(SYSTEM ${OSMIUM_INCLUDE_DIRS})

find_package(Boost 1.55.0)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

include(common)

enable_testing()

#----------------------------------------------------------------------

set(PROG export_to_wkt)
file(GLOB SOURCES *.cpp *.hpp)
add_executable(${PROG} ${SOURCES})
target_link_libraries(${PROG} ${Boost_LIBRARIES} ${OSMIUM_LIBRARIES})
set_pthread_on_target(${PROG})

add_test(export_to_wkt export_to_wkt ${CMAKE_CURRENT_SOURCE_DIR}/test/node.osm)

set_tests_properties(export_to_wkt PROPERTIES
                     PASS_REGULAR_EXPRESSION "n24960505 POINT\\(8\\.8720536 53\\.096629\\)\n"
)


#----------------------------------------------------------------------
