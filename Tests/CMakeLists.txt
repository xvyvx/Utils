include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/BuildUtils/UnitTestHelper.cmake)

AddExecutableTarget(UtilsTest SRC TestStub.cpp LinearBufferTest.cpp CircularBufferTest.cpp BufferCacheTest.cpp
    TlsTest.cpp TcpChannelTest.cpp SerialportChannelTest.cpp TimerCacheTest.cpp PathHelperTest.cpp
    BinaryHelperTest.cpp DiagnosticsTest.cpp ThreadPoolTest.cpp BlackMagicsTest.cpp WaitEventTest.cpp 
    UnixSignalHelperTest.cpp
    PRIVATE_INC ${CMAKE_CURRENT_SOURCE_DIR}/../Utils
    DEPS Boost::chrono Boost::unit_test_framework Boost::timer Utils)

AddTestCaseToTarget(UtilsTest TESTCASE CircularBufferCacheTest FILTER BufferCacheTest/CircularBufferCacheTest
    TESTCASE LinearBufferCacheTest FILTER BufferCacheTest/LinearBufferCacheTest
    TESTCASE LinearBufferCacheVectorTest FILTER BufferCacheTest/LinearBufferCacheVectorTest
    TESTCASE LinearBufferCacheNoTrivialWrapperTest FILTER BufferCacheTest/LinearBufferCacheNoTrivialWrapperTest
    TESTCASE CircularBufferGeneralTest FILTER CircularBufferTest/GeneralTest
    TESTCASE CircularBufferIteratorTest FILTER CircularBufferTest/IteratorTest
    TESTCASE CircularBufferCopyCtrlTest FILTER CircularBufferTest/CopyCtrlTest
    TESTCASE CircularBufferLogicOperatorTest FILTER CircularBufferTest/LogicOperatorTest
    TESTCASE LinearBufferCopyCtrlTest FILTER LinearBufferTest/CopyCtrlTest
    TESTCASE LinearBufferReadWriteTest FILTER LinearBufferTest/ReadWriteTest
    TESTCASE TcpChannelGeneralTest FILTER TcpChannelTest/GeneralTest
    TESTCASE DeadlineTimerCacherGeneralTest FILTER TimerCacheTest/DeadlineTimerCacheTest
    TESTCASE SteadyTimerCacherGeneralTest FILTER TimerCacheTest/SteadyTimerCacheTest
    TESTCASE SerialPortChannelTest FILTER SerialPortChannelTest/GeneralTest COND UNIX
    TESTCASE TlsGeneralTest FILTER TlsTest/GeneralTest
    TESTCASE PathHelperDeployPathTest FILTER PathHelperTest/DeployPathTest
    TESTCASE PathHelperExecutablePathTest FILTER PathHelperTest/ExecutablePathTest
    TESTCASE BinaryHelperGeneralTest FILTER BinaryHelperTest/GeneralTest
    TESTCASE DiagnosticsTest FILTER DiagnosticsTest/GetCPUUsageTest
    TESTCASE ThreadPoolTest FILTER ThreadPoolTest/GeneralTest
    TESTCASE BlackMagicsTest FILTER BlackMagicsTest/GeneralTest
    TESTCASE WaitEventTest FILTER WaitEventTest/GeneralTest
    TESTCASE UnixSignalHelperDiscardChildInfoTest FILTER UnixSignalHelperTest/DiscardChildInfoTest COND UNIX)