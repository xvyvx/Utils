SET(TestsSources LinearBufferTest.cpp CircularBufferTest.cpp BufferCacheTest.cpp TlsTest.cpp TcpChannelTest.cpp)

ADD_EXECUTABLE(UtilsTest ${TestsSources})
TARGET_INCLUDE_DIRECTORIES(UtilsTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Utils)
TARGET_LINK_LIBRARIES(UtilsTest Boost::unit_test_framework Utils)