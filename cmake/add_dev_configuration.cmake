#-----------------------------------------------------------------------------
#
#  Modification of configuration types, adding Dev configuration.
#  should be included before project() to avoid cmake problems with MSVC
#
#-----------------------------------------------------------------------------

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel;Dev"
    CACHE STRING
    "List of available configuration types"
    FORCE)
