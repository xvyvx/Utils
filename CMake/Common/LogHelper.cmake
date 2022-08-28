include_guard(DIRECTORY)

macro(LogDebug)
    LogCore(DEBUG ${ARGV})
endmacro()

function(LogCore level)
    get_filename_component(currentListFile ${CMAKE_CURRENT_LIST_FILE} NAME)
    message(${level} "[${level}][${currentListFile}] - ${ARGN}")
endfunction()