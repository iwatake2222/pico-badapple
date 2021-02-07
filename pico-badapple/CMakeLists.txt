cmake_minimum_required(VERSION 3.0)

# Create project
set(ProjectName "main")
project(${ProjectName})

# Create executable file
add_executable(${ProjectName}
	Main.cpp
	OledSh1106.cpp
	OledSh1106.h
	font.cpp
	VideoData.h
)
 