set(TestsSources LinearBufferTest.cpp CircularBufferTest.cpp BufferCacheTest.cpp TlsTest.cpp TcpChannelTest.cpp SerialportChannelTest.cpp PathHelperTest.cpp BinaryHelperTest.cpp
    DiagnosticsTest.cpp ThreadPoolTest.cpp BlackMagicsTest.cpp WaitEventTest.cpp)

add_executable(UtilsTest ${TestsSources})
EnableGNUStyleStrip(TARGET UtilsTest)
target_include_directories(UtilsTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Utils)
target_link_libraries(UtilsTest Boost::unit_test_framework Boost::timer Utils)

macro(AddToTest TestName Filter)
    add_test(NAME ${TestName} COMMAND UtilsTest --run_test=${Filter} --log_level=message)
    set_tests_properties(${TestName} PROPERTIES PASS_REGULAR_EXPRESSION "No errors detected")
endmacro(AddToTest)

AddToTest(CircularBufferCacheTest BufferCacheTest/CircularBufferCacheTest)
AddToTest(LinearBufferCacheTest BufferCacheTest/LinearBufferCacheTest)
AddToTest(LinearBufferCacheHelperTest BufferCacheTest/LinearBufferCacheHelperTest)

AddToTest(CircularBufferGeneralTest CircularBufferTest/GeneralTest)
AddToTest(CircularBufferIteratorTest CircularBufferTest/IteratorTest)
AddToTest(CircularBufferCopyCtrlTest CircularBufferTest/CopyCtrlTest)
AddToTest(CircularBufferLogicOperatorTest CircularBufferTest/LogicOperatorTest)

AddToTest(LinearBufferCopyCtrlTest LinearBufferTest/CopyCtrlTest)
AddToTest(ReadWriteTest LinearBufferTest/ReadWriteTest)

AddToTest(TcpChannelGeneralTest TcpChannelTest/GeneralTest)

if(UNIX)
    AddToTest(SerialPortChannelTest SerialPortChannelTest/GeneralTest)
endif()

AddToTest(TlsGeneralTest TlsTest/GeneralTest)

AddToTest(PathHelperDeployPathTest PathHelperTest/DeployPathTest)
AddToTest(PathHelperExecutablePathTest PathHelperTest/ExecutablePathTest)

AddToTest(BinaryHelperGeneralTest BinaryHelperTest/GeneralTest)

AddToTest(DiagnosticsTest DiagnosticsTest/GetCPUUsageTest)

AddToTest(ThreadPoolTest ThreadPoolTest/GeneralTest)

AddToTest(BlackMagicsTest BlackMagicsTest/GeneralTest)

AddToTest(WaitEventTest WaitEventTest/GeneralTest)