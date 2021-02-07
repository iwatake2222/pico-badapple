cmake_minimum_required(VERSION 3.12)

# CMake options
set(BUILD_FOR_RASPBERRY_PI_PICO on CACHE BOOL "Build for Raspberry Pi Pico [on/off]")

if(BUILD_FOR_RASPBERRY_PI_PICO)
	set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-sdk)
	include(pico_sdk_import.cmake)
endif()

# Create project
set(ProjectName "pico-badapple")
project(${ProjectName})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(BUILD_FOR_RASPBERRY_PI_PICO)
	pico_sdk_init()
	add_definitions(-UPLATFORM_LINUX -DPLATFORM_PIPICO)
else()
	add_definitions(-DPLATFORM_LINUX -UPLATFORM_PIPICO)
endif()

add_subdirectory(pico-badapple)
