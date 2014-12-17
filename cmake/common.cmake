#----------------------------------------------------------------------
#
#  CMake config used in all projects in this repository.
#
#----------------------------------------------------------------------

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

set(CMAKE_CXX_FLAGS_NONE "-O3 -g"
    CACHE STRING "Flags used by the compiler during all builds."
    FORCE)

set(CMAKE_CXX_FLAGS_DEV "-O3 -g ${OSMIUM_WARNING_OPTIONS}"
    CACHE STRING "Flags used by the compiler during developer builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEV ""
    CACHE STRING "Flags used by the linker during developer builds."
    FORCE)
mark_as_advanced(
    CMAKE_CXX_FLAGS_DEV
    CMAKE_EXE_LINKER_FLAGS_DEV
)

set(CMAKE_CONFIGURATION_TYPES "Debug Release RelWithDebInfo MinSizeRel Dev")

# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: ${CMAKE_CONFIGURATION_TYPES}."
    FORCE)

