cmake_minimum_required(VERSION 3.0)

# CMake options
set(BUILD_FOR_RASPBERRY_PI_PICO on CACHE BOOL "Build for Raspberry Pi Pico [on/off]")

if(BUILD_FOR_RASPBERRY_PI_PICO)
	set(PICO_SDK_PATH ../pico-sdk)
	include(pico_sdk_import.cmake)
	pico_sdk_init()
	add_definitions(-UPLATFORM_LINUX -DPLATFORM_PIPICO)
else()
	add_definitions(-DPLATFORM_LINUX -UPLATFORM_PIPICO)
endif()

# Create project
set(ProjectName "main")
project(${ProjectName})
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(pico-badapple)
