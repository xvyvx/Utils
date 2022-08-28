include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/ToolchainHelper.cmake)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_COMPILER_AR ar)
set(CMAKE_C_COMPILER_RANLIB ranlib)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_COMPILER_AR ar)
set(CMAKE_CXX_COMPILER_RANLIB ranlib)
set(CMAKE_ASM_COMPILER gcc)
if(NativeToolchainVersion)
    set(CMAKE_C_COMPILER gcc-${NativeToolchainVersion})
    set(CMAKE_CXX_COMPILER g++-${NativeToolchainVersion})
    set(CMAKE_ASM_COMPILER gcc-${NativeToolchainVersion})
endif()

list(APPEND CodeCoverageCompileParam --coverage)
list(APPEND CodeCoverageLinkParam --coverage)

FindNativeToolChainProg(objcopy)
FindNativeToolChainProg(strip)