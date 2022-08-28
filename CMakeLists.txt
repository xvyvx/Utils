cmake_minimum_required(VERSION 3.17.0)

project(Utils)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN")
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

include(${PROJECT_SOURCE_DIR}/CMake/BuildUtils/BuildHelper.cmake)

option(ENABLE_ODBC_DATABASE_UTILS "Enable or disable odbc database utilities(ON/OFF),default is OFF" OFF)
option(ENABLE_OCI_DATABASE_UTILS "Enable or disable oci database utilities(ON/OFF),default is OFF" OFF)
find_package(Boost REQUIRED COMPONENTS system thread regex filesystem program_options unit_test_framework timer)
find_package(log4cplus REQUIRED)
find_package(otl REQUIRED)
if(UNIX)
	find_package(libbsd REQUIRED)
endif()

add_subdirectory(Utils)

option(BUILD_TESTS "Wether to build test cases(ON/OFF),default is ON." ON)
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(Tests)
endif()
