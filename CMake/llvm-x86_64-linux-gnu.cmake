include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/ToolchainUtils.cmake)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(NativeToolchainTriplet "llvm")
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_AR ${NativeToolchainTriplet}-ar)
set(CMAKE_C_COMPILER_RANLIB ${NativeToolchainTriplet}-ranlib)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_AR ${NativeToolchainTriplet}-ar)
set(CMAKE_CXX_COMPILER_RANLIB ${NativeToolchainTriplet}-ranlib)
set(CMAKE_ASM_COMPILER clang)
if(NativeToolchainVersion)
    set(CMAKE_C_COMPILER clang-${NativeToolchainVersion})
    set(CMAKE_CXX_COMPILER clang++-${NativeToolchainVersion})
    set(CMAKE_ASM_COMPILER clang-${NativeToolchainVersion})
endif()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -stdlib=libc++)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-fuse-ld=lld --rtlib=compiler-rt -lunwind")
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-fuse-ld=lld --rtlib=compiler-rt -lunwind")
list(APPEND CodeCoverageCompileParam -fprofile-instr-generate -fcoverage-mapping)
list(APPEND CodeCoverageLinkParam -fprofile-instr-generate)

FindNativeToolChainProg(objcopy)
FindNativeToolChainProg(strip)