include(GNUInstallDirs)
set(InstIncludeDir ${CMAKE_INSTALL_INCLUDEDIR})
set(InstLibDir ${CMAKE_INSTALL_LIBDIR})

set(UtilsVersion 1.0.0)
set(UtilsSoVersion 1)

AddSharedLibraryTarget(Utils SRC AppEntry/IProgressReporter.cpp AppEntry/WinSvcProgressReporter.cpp
    AppEntry/SystemdProgressReporter.cpp
    Buffer/BinaryHelper.cpp Buffer/CircularBuffer.cpp Buffer/CircularBufferCache.cpp Buffer/LinearBuffer.cpp
    Buffer/LinearBufferCache.cpp
    Channel/Common/IAsyncChannel.cpp Channel/Common/IAsyncChannelHandler.cpp Channel/SerialPort/SerialPortChannel.cpp
    Channel/Tcp/TcpV4Channel.cpp Channel/Tcp/TcpV4Listener.cpp Channel/Tcp/TcpV4PassiveChannel.cpp
    Common/PathHelper.cpp Common/WinSrvHelper.cpp
    Concurrent/Timer/SteadyTimerCache.cpp Concurrent/Timer/DeadlineTimerCache.cpp Concurrent/BlackMagics.cpp
    Concurrent/ThreadPool.cpp Concurrent/WaitEvent.cpp
    Database/SQL/Common/ISqlDatabase.cpp Database/SQL/Common/SqlDatabasePool.cpp
    Database/SQL/ODBC/ODBCSqlDatabase.cpp Database/SQL/OCI/OCISqlDatabase.cpp
    Diagnostics/DiagnosticsHelper.cpp
    VER ${UtilsVersion} SOVER ${UtilsSoVersion} COVERAGE_BUILD
    INTERFACE_INC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<INSTALL_INTERFACE:${InstIncludeDir}>
    PUBLIC_DEFS USE_ODBC_DATABASE_UTILS COND UNIX AND ${ENABLE_ODBC_DATABASE_UTILS}
    PUBLIC_DEFS USE_OCI_DATABASE_UTILS COND UNIX AND ${ENABLE_OCI_DATABASE_UTILS}
    PUBLIC_DEFS USE_ODBC_DATABASE_UTILS COND WIN32 AND ${ENABLE_ODBC_DATABASE_UTILS}
    PUBLIC_DEFS USE_OCI_DATABASE_UTILS COND WIN32 AND ${ENABLE_OCI_DATABASE_UTILS}
    DEPS Boost::thread Boost::system Boost::regex Boost::filesystem Boost::program_options log4cplus::log4cplus otl
    DEPS "pthread" bsd dl COND UNIX
    DEPS "odbc" COND UNIX AND ${ENABLE_ODBC_DATABASE_UTILS}
    DEPS "oci" COND UNIX AND ${ENABLE_OCI_DATABASE_UTILS}
    DEPS "odbc32" "odbccp32" COND WIN32 AND ${ENABLE_ODBC_DATABASE_UTILS}
    DEPS "oci" COND WIN32 AND ${ENABLE_OCI_DATABASE_UTILS})

include(CMakePackageConfigHelpers)
export(TARGETS Utils NAMESPACE Utils:: FILE ${PROJECT_BINARY_DIR}/UtilsTargets.cmake)
install(TARGETS Utils EXPORT UtilsTargets ARCHIVE DESTINATION ${InstLibDir} COMPONENT Development
    LIBRARY DESTINATION ${InstLibDir} COMPONENT Runtime
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Utils DESTINATION ${InstIncludeDir} FILES_MATCHING PATTERN *.h PATTERN *Impl.h EXCLUDE)
install(FILES ${PROJECT_SOURCE_DIR}/CMake/Findotl.cmake DESTINATION ${InstLibDir}/cmake/Utils)
if(UNIX)
    install(FILES ${PROJECT_SOURCE_DIR}/CMake/Findlibbsd.cmake DESTINATION ${InstLibDir}/cmake/Utils)
endif()
configure_package_config_file(${PROJECT_SOURCE_DIR}/CMake/UtilsConfig.cmake.in ${PROJECT_BINARY_DIR}/Utils/UtilsConfig.cmake
    INSTALL_DESTINATION ${InstLibDir}/cmake/Utils PATH_VARS InstIncludeDir InstLibDir)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/Utils/UtilsConfigVersion.cmake" VERSION ${UtilsVersion} COMPATIBILITY AnyNewerVersion)
set(ExportFileName UtilsTargets.cmake)
install(EXPORT UtilsTargets DESTINATION ${InstLibDir}/cmake/Utils FILE ${ExportFileName})
install(FILES ${PROJECT_BINARY_DIR}/Utils/UtilsConfigVersion.cmake ${PROJECT_BINARY_DIR}/Utils/UtilsConfig.cmake DESTINATION ${InstLibDir}/cmake/Utils)