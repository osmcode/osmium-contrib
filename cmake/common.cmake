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
add_compile_options(-std=${USE_CPP_VERSION})

