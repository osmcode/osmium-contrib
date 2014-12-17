#----------------------------------------------------------------------
#
#  CMake config used in all projects in this repository.
#
#----------------------------------------------------------------------

include(OsmiumOptions)

include(CppcheckTarget)
add_cppcheck_target(*.*pp)

#----------------------------------------------------------------------
#
#  Decide which C++ version to use (Minimum/default: C++11).
#
#----------------------------------------------------------------------

if(NOT USE_CPP_VERSION)
    set(USE_CPP_VERSION c++11)
endif()
message(STATUS "Use C++ version: ${USE_CPP_VERSION}")
# following only available from cmake 2.8.12:
#   add_compile_options(-std=${USE_CPP_VERSION})
# so using this instead:
add_definitions(-std=${USE_CPP_VERSION})

#----------------------------------------------------------------------

if(NOT MSVC)
    add_definitions(${OSMIUM_WARNING_OPTIONS})
endif()

