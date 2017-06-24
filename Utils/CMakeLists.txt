SET(UtilSources 
	AppEntry/IProgressReporter.cpp AppEntry/WinSvcProgressReporter.cpp
	Buffer/BinaryHelper.cpp Buffer/CircularBuffer.cpp Buffer/CircularBufferCache.cpp Buffer/LinearBuffer.cpp Buffer/LinearBufferCache.cpp
	Channel/Common/IAsyncChannel.cpp Channel/Common/IAsyncChannelHandler.cpp Channel/SerialPort/SerialPortChannel.cpp
	Channel/Tcp/TcpV4Channel.cpp Channel/Tcp/TcpV4Listener.cpp Channel/Tcp/TcpV4PassiveChannel.cpp
	Common/PathHelper.cpp
	Concurrent/BlackMagics.cpp Concurrent/ThreadPool.cpp Concurrent/WaitEvent.cpp
	Database/SQL/Common/ISqlDatabase.cpp Database/SQL/Common/SqlDatabasePool.cpp Database/SQL/ODBC/ODBCSqlDatabase.cpp
	Database/SQL/OCI/OCISqlDatabase.cpp)

ADD_LIBRARY(Utils SHARED ${UtilSources})
SET_TARGET_PROPERTIES(Utils PROPERTIES VERSION 1.0 SOVERSION 1)
TARGET_COMPILE_DEFINITIONS(Utils INTERFACE -DUTILS_IMPORTS PRIVATE -DUTILS_EXPORTS)
IF(UNIX)
	TARGET_LINK_LIBRARIES(Utils "pthread" bsd Boost::thread Boost::system Boost::regex Boost::filesystem Boost::program_options log4cplus)
	TARGET_COMPILE_OPTIONS(Utils PUBLIC -std=c++11 PRIVATE -fvisibility=hidden -fPIC -Wall) 
	IF(ENABLE_ODBC_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_ODBC_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils "odbc")
	ENDIF()
	IF(ENABLE_OCI_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_OCI_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils "oci")
	ENDIF()
ELSEIF(WIN32)
	TARGET_LINK_LIBRARIES(Utils Boost::thread Boost::system Boost::regex Boost::filesystem Boost::program_options log4cplus)
	TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -D_WIN32_WINNT=0x0600 PRIVATE -D_SCL_SECURE_NO_WARNINGS)
	IF(ENABLE_ODBC_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_ODBC_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils "odbc32" "odbccp32")
	ENDIF()
	IF(ENABLE_OCI_DATABASE_UTILS)
		TARGET_COMPILE_DEFINITIONS(Utils PUBLIC -DUSE_OCI_DATABASE_UTILS)
		TARGET_LINK_LIBRARIES(Utils "oci")
	ENDIF()
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported platform.")
ENDIF()