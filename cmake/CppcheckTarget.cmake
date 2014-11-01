#----------------------------------------------------------------------
#
#  Optional "cppcheck" target that checks C++ code
#
#----------------------------------------------------------------------
function(add_cppcheck_target _var)

  message(STATUS "Looking for cppcheck")
  find_program(CPPCHECK cppcheck)

  if(CPPCHECK)
    message(STATUS "Looking for cppcheck - found")
    set(CPPCHECK_OPTIONS --enable=warning,style,performance,portability,information,missingInclude)

    # cpp doesn't find system includes for some reason, suppress that report
    set(CPPCHECK_OPTIONS ${CPPCHECK_OPTIONS} --suppress=missingIncludeSystem)

    add_custom_target(cppcheck ${CPPCHECK} --std=c++11 ${CPPCHECK_OPTIONS} ${_var})
  else()
    message(STATUS "Looking for cppcheck - not found")
    message(STATUS "  Make target cppcheck not available")
  endif(CPPCHECK)
endfunction()
