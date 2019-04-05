SET(UtilSources AppEntry/IProgressReporter.cpp AppEntry/WinSvcProgressReporter.cpp AppEntry/SystemdProgressReporter.cpp
	Buffer/BinaryHelper.cpp Buffer/CircularBuffer.cpp Buffer/CircularBufferCache.cpp Buffer/LinearBuffer.cpp Buffer/LinearBufferCache.cpp
	Channel/Common/IAsyncChannel.cpp Channel/Common/IAsyncChannelHandler.cpp Channel/SerialPort/SerialPortChannel.cpp
	Channel/Tcp/TcpV4Channel.cpp Channel/Tcp/TcpV4Listener.cpp Channel/Tcp/TcpV4PassiveChannel.cpp
	Common/PathHelper.cpp Common/WinSrvHelper.cpp
	Concurrent/BlackMagics.cpp Concurrent/ThreadPool.cpp Concurrent/WaitEvent.cpp
	Database/SQL/Common/ISqlDatabase.cpp Database/SQL/Common/SqlDatabasePool.cpp Database/SQL/ODBC/ODBCSqlDatabase.cpp
	Database/SQL/OCI/OCISqlDatabase.cpp
	Diagnostics/DiagnosticsHelper.cpp)

	INCLUDE(GNUInstallDirs)
	SET(InstIncludeDir ${CMAKE_INSTALL_INCLUDEDIR})
	SET(InstLibDir ${CMAKE_INSTALL_LIBDIR})

SET(UtilsVersion 1.0)
SET(UtilsSoVersion 1)
ADD_LIBRARY(Utils SHARED ${UtilSources})
TARGET_INCLUDE_DIRECTORIES(Utils INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<INSTALL_INTERFACE:${InstIncludeDir}>)
SET_TARGET_PROPERTIES(Utils PROPERTIES VERSION ${UtilsVersion} SOVERSION ${UtilsSoVersion} CXX_VISIBILITY_PRESET hidden)
TARGET_COMPILE_DEFINITIONS(Utils INTERFACE -DUTILS_IMPORTS PRIVATE -DUTILS_EXPORTS)
TARGET_COMPILE_FEATURES(Utils PUBLIC cxx_std_11)
TARGET_LINK_LIBRARIES(Utils PUBLIC Boost::thread Boost::system Boost::regex Boost::filesystem Boost::program_options log4cplus otl)
IF(UNIX)
	TARGET_LINK_LIBRARIES(Utils PRIVATE "pthread" bsd)
	TARGET_COMPILE_OPTIONS(Utils PRIVATE -Wall) 
	IF(ENABLE_ODBC_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_ODBC_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils PUBLIC "odbc")
	ENDIF()
	IF(ENABLE_OCI_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_OCI_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils PUBLIC "oci")
	ENDIF()
ELSEIF(WIN32)
	TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -D_WIN32_WINNT=0x0600 PRIVATE -D_SCL_SECURE_NO_WARNINGS)
	TARGET_COMPILE_OPTIONS(Utils PRIVATE $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>,$<CONFIG:RELWITHDEBINFO>>:/GL>)
	SET_PROPERTY(TARGET Utils APPEND PROPERTY LINK_FLAGS_RELEASE " /LTCG /OPT:REF")
	SET_PROPERTY(TARGET Utils APPEND PROPERTY LINK_FLAGS_MINSIZEREL " /LTCG /OPT:REF")
	SET_PROPERTY(TARGET Utils APPEND PROPERTY LINK_FLAGS_RELWITHDEBINFO " /LTCG /OPT:REF")
	IF(ENABLE_ODBC_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_ODBC_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils PUBLIC "odbc32" "odbccp32")
	ENDIF()
	IF(ENABLE_OCI_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_OCI_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils PUBLIC "oci")
	ENDIF()
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported platform.")
ENDIF()

INCLUDE(CMakePackageConfigHelpers)
EXPORT(TARGETS Utils NAMESPACE Utils:: FILE ${PROJECT_BINARY_DIR}/UtilsTargets.cmake)
INSTALL(TARGETS Utils EXPORT UtilsTargets ARCHIVE DESTINATION ${InstLibDir} LIBRARY DESTINATION ${InstLibDir}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/Utils DESTINATION ${InstIncludeDir} FILES_MATCHING PATTERN *.h PATTERN *Impl.h EXCLUDE)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/CMake/FindBoost.cmake ${PROJECT_SOURCE_DIR}/CMake/Findlog4cplus.cmake ${PROJECT_SOURCE_DIR}/CMake/Findotl.cmake
	DESTINATION ${InstLibDir}/cmake/Utils)
IF(UNIX)
	INSTALL(FILES ${PROJECT_SOURCE_DIR}/CMake/Findlibbsd.cmake DESTINATION ${InstLibDir}/cmake/Utils)
ENDIF()
configure_package_config_file(${PROJECT_SOURCE_DIR}/CMake/UtilsConfig.cmake.in ${PROJECT_BINARY_DIR}/Utils/UtilsConfig.cmake
	INSTALL_DESTINATION ${InstLibDir}/cmake/Utils PATH_VARS InstIncludeDir InstLibDir)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/Utils/UtilsConfigVersion.cmake" VERSION ${UtilsVersion} COMPATIBILITY AnyNewerVersion)
SET(ExportFileName UtilsTargets.cmake)
INSTALL(EXPORT UtilsTargets DESTINATION ${InstLibDir}/cmake/Utils FILE ${ExportFileName})
INSTALL(FILES ${PROJECT_BINARY_DIR}/Utils/UtilsConfigVersion.cmake ${PROJECT_BINARY_DIR}/Utils/UtilsConfig.cmake DESTINATION ${InstLibDir}/cmake/Utils)