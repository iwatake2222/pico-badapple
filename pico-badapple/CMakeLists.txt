# Create executable file
add_executable(${ProjectName}
	Main.cpp
	OledSh1106.cpp
	OledSh1106.h
	font.cpp
	VideoData.h
)

if(BUILD_FOR_RASPBERRY_PI_PICO)
	target_link_libraries(${ProjectName} pico_stdlib hardware_i2c)
	pico_add_extra_outputs(${ProjectName})
endif()
