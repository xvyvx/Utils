SET(TestsSources LinearBufferTest.cpp CircularBufferTest.cpp BufferCacheTest.cpp TlsTest.cpp TcpChannelTest.cpp PathHelperTest.cpp BinaryHelperTest.cpp)

ADD_EXECUTABLE(UtilsTest ${TestsSources})
TARGET_INCLUDE_DIRECTORIES(UtilsTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Utils)
TARGET_LINK_LIBRARIES(UtilsTest Boost::unit_test_framework Utils)

MACRO(AddToTest TestName Filter)
	ADD_TEST(NAME ${TestName} COMMAND UtilsTest --run_test=${Filter})
	SET_TESTS_PROPERTIES(${TestName} PROPERTIES PASS_REGULAR_EXPRESSION "No errors detected")
ENDMACRO(AddToTest)

AddToTest(CircularBufferCacheTest BufferCacheTest/CircularBufferCacheTest)
AddToTest(LinearBufferCacheTest BufferCacheTest/LinearBufferCacheTest)

AddToTest(CircularBufferGeneralTest CircularBufferTest/GeneralTest)
AddToTest(CircularBufferIteratorTest CircularBufferTest/IteratorTest)
AddToTest(CircularBufferCopyCtrlTest CircularBufferTest/CopyCtrlTest)

AddToTest(LinearBufferCopyCtrlTest LinearBufferTest/CopyCtrlTest)
AddToTest(ReadWriteTest LinearBufferTest/ReadWriteTest)

AddToTest(TcpChannelGeneralTest TcpChannelTest/GeneralTest)

AddToTest(TlsGeneralTest TlsTest/GeneralTest)

AddToTest(PathHelperGeneralTest PathHelperTest/GeneralTest)

AddToTest(BinaryHelperGeneralTest BinaryHelperTest/GeneralTest)