set(UtilSources AppEntry/IProgressReporter.cpp AppEntry/WinSvcProgressReporter.cpp AppEntry/SystemdProgressReporter.cpp
    Buffer/BinaryHelper.cpp Buffer/CircularBuffer.cpp Buffer/CircularBufferCache.cpp Buffer/LinearBuffer.cpp Buffer/LinearBufferCache.cpp
    Channel/Common/IAsyncChannel.cpp Channel/Common/IAsyncChannelHandler.cpp Channel/SerialPort/SerialPortChannel.cpp
    Channel/Tcp/TcpV4Channel.cpp Channel/Tcp/TcpV4Listener.cpp Channel/Tcp/TcpV4PassiveChannel.cpp
    Common/PathHelper.cpp Common/WinSrvHelper.cpp
    Concurrent/Timer/SteadyTimerCache.cpp Concurrent/Timer/DeadlineTimerCache.cpp Concurrent/BlackMagics.cpp
    Concurrent/ThreadPool.cpp Concurrent/WaitEvent.cpp
    Database/SQL/Common/ISqlDatabase.cpp Database/SQL/Common/SqlDatabasePool.cpp Database/SQL/ODBC/ODBCSqlDatabase.cpp
    Database/SQL/OCI/OCISqlDatabase.cpp
    Diagnostics/DiagnosticsHelper.cpp)

include(GNUInstallDirs)
set(InstIncludeDir ${CMAKE_INSTALL_INCLUDEDIR})
set(InstLibDir ${CMAKE_INSTALL_LIBDIR})

set(UtilsVersion 1.0)
set(UtilsSoVersion 1)
add_library(Utils SHARED ${UtilSources})
EnableCodeCoverageBuild(TARGET Utils)
EnableGNUStyleStrip(TARGET Utils)
target_include_directories(Utils INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<INSTALL_INTERFACE:${InstIncludeDir}>)
set_target_properties(Utils PROPERTIES VERSION ${UtilsVersion} SOVERSION ${UtilsSoVersion} CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(Utils INTERFACE -DUTILS_IMPORTS PRIVATE -DUTILS_EXPORTS)
target_compile_features(Utils PUBLIC cxx_std_14)
target_link_libraries(Utils PUBLIC Boost::thread Boost::system Boost::regex Boost::filesystem Boost::program_options log4cplus otl)
if(UNIX)
    target_link_libraries(Utils PRIVATE "pthread" bsd dl)
    target_compile_options(Utils PRIVATE -Wall) 
    if(ENABLE_ODBC_DATABASE_UTILS)
        target_compile_definitions(Utils PUBLIC -DUSE_ODBC_DATABASE_UTILS)
        target_link_libraries(Utils PUBLIC "odbc")
    endif()
    if(ENABLE_OCI_DATABASE_UTILS)
        target_compile_definitions(Utils PUBLIC -DUSE_OCI_DATABASE_UTILS)
        target_link_libraries(Utils PUBLIC "oci")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(TargetOutputName "libUtils.so.${UtilsVersion}")
        add_custom_command(TARGET Utils POST_BUILD
            COMMAND objcopy --only-keep-debug ./${TargetOutputName} ./${TargetOutputName}.debug COMMAND strip --strip-debug --strip-unneeded ./${TargetOutputName}
            COMMAND objcopy --add-gnu-debuglink=${TargetOutputName}.debug ./${TargetOutputName}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)
    endif()
elseif(WIN32)
    target_compile_definitions(Utils PUBLIC -D_WIN32_WINNT=0x0600 PRIVATE -D_SCL_SECURE_NO_WARNINGS)
    target_compile_options(Utils PRIVATE $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>,$<CONFIG:RELWITHDEBINFO>>:/GL>)
    set_property(TARGET Utils APPEND PROPERTY LINK_FLAGS_RELEASE " /LTCG /OPT:REF")
    set_property(TARGET Utils APPEND PROPERTY LINK_FLAGS_MINSIZEREL " /LTCG /OPT:REF")
    set_property(TARGET Utils APPEND PROPERTY LINK_FLAGS_RELWITHDEBINFO " /LTCG /OPT:REF")
    if(ENABLE_ODBC_DATABASE_UTILS)
        target_compile_definitions(Utils PUBLIC -DUSE_ODBC_DATABASE_UTILS)
        target_link_libraries(Utils PUBLIC "odbc32" "odbccp32")
    endif()
    if(ENABLE_OCI_DATABASE_UTILS)
        target_compile_definitions(Utils PUBLIC -DUSE_OCI_DATABASE_UTILS)
        target_link_libraries(Utils PUBLIC "oci")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform.")
endif()

include(CMakePackageConfigHelpers)
export(TARGETS Utils NAMESPACE Utils:: FILE ${PROJECT_BINARY_DIR}/UtilsTargets.cmake)
install(TARGETS Utils EXPORT UtilsTargets ARCHIVE DESTINATION ${InstLibDir} LIBRARY DESTINATION ${InstLibDir}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Utils DESTINATION ${InstIncludeDir} FILES_MATCHING PATTERN *.h PATTERN *Impl.h EXCLUDE)
install(FILES ${PROJECT_SOURCE_DIR}/CMake/Findlog4cplus.cmake ${PROJECT_SOURCE_DIR}/CMake/Findotl.cmake DESTINATION ${InstLibDir}/cmake/Utils)
if(UNIX)
    install(FILES ${PROJECT_SOURCE_DIR}/CMake/Findlibbsd.cmake DESTINATION ${InstLibDir}/cmake/Utils)
endif()
configure_package_config_file(${PROJECT_SOURCE_DIR}/CMake/UtilsConfig.cmake.in ${PROJECT_BINARY_DIR}/Utils/UtilsConfig.cmake
    INSTALL_DESTINATION ${InstLibDir}/cmake/Utils PATH_VARS InstIncludeDir InstLibDir)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/Utils/UtilsConfigVersion.cmake" VERSION ${UtilsVersion} COMPATIBILITY AnyNewerVersion)
set(ExportFileName UtilsTargets.cmake)
install(EXPORT UtilsTargets DESTINATION ${InstLibDir}/cmake/Utils FILE ${ExportFileName})
install(FILES ${PROJECT_BINARY_DIR}/Utils/UtilsConfigVersion.cmake ${PROJECT_BINARY_DIR}/Utils/UtilsConfig.cmake DESTINATION ${InstLibDir}/cmake/Utils)