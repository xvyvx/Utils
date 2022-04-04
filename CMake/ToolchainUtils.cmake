include_guard(GLOBAL)

function(FindNativeToolChainProg progName)
    set(progRealName ${progName})
    if(NativeToolchainTriplet)
        set(progRealName ${NativeToolchainTriplet}-${progName})
    endif()
    find_program(toolchainNativeProgPath_${progName} ${progRealName} ONLY_CMAKE_FIND_ROOT_PATH)
    if(NOT toolchainNativeProgPath_${progName})
        message(FATAL_ERROR "Can't find toolchain programe(${progRealName}),\
            please set CMAKE_FIND_ROOT_PATH variable correctly.")
    endif()
    add_executable("Toolchain::${progName}" IMPORTED GLOBAL)
    set_target_properties("Toolchain::${progName}" PROPERTIES IMPORTED_LOCATION
        ${toolchainNativeProgPath_${progName}})
endfunction()