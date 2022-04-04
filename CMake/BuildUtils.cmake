include_guard(DIRECTORY)

macro(CheckGNUStyleToolchain)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
        set(isGNUStyleToolchain TRUE)
    else()
        set(isGNUStyleToolchain FALSE)
    endif()
endmacro()

function(EnableGNUStyleStrip)
    CheckGNUStyleToolchain()
    if(isGNUStyleToolchain)
        set(dbgOptionalArgNames "")
        set(dbgOneValArgNames "TARGET")
        set(dbgMultiValArgNames "")
        cmake_parse_arguments(dbg "${dbgOptionalArgNames}" "${dbgOneValArgNames}" "${dbgMultiValArgNames}" ${ARGN})
        add_custom_command(TARGET ${dbg_TARGET} POST_BUILD
            COMMAND Toolchain::objcopy --only-keep-debug ./$<TARGET_FILE_NAME:${dbg_TARGET}> ./$<TARGET_FILE_NAME:${dbg_TARGET}>.debug
            COMMAND Toolchain::strip --strip-debug --strip-unneeded ./$<TARGET_FILE_NAME:${dbg_TARGET}>
            COMMAND Toolchain::objcopy --add-gnu-debuglink=$<TARGET_FILE_NAME:${dbg_TARGET}>.debug ./$<TARGET_FILE_NAME:${dbg_TARGET}>
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
    endif()
endfunction()

function(EnableCodeCoverageBuild)
    if(CodeCoverageCompileParam AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(covBuildOptionalArgNames "")
        set(covBuildOneValArgNames "TARGET")
        set(covBuildMultiValArgNames "")
        cmake_parse_arguments(covBuild "${covBuildOptionalArgNames}" "${covBuildOneValArgNames}"
            "${covBuildMultiValArgNames}" ${ARGN})
        if(TARGET ${covBuild_TARGET})
            target_compile_options(${covBuild_TARGET} PRIVATE ${CodeCoverageCompileParam})
            if(CodeCoverageLinkParam)
                target_link_options(${covBuild_TARGET} PRIVATE ${CodeCoverageLinkParam})
            endif()
        else()
            message(FATAL_ERROR "EnableCodeCoverageSupport must be called with a target.")
        endif()
    else()
        message(STATUS "Code coverage build is not supportted by platform.")
    endif()
endfunction()