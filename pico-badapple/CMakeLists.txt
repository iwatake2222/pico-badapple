set(BinName "pico-badapple")
add_executable(${BinName}
	main.cpp
	oled.cpp
	oled.h
	my_font.cpp
	my_font.h
	video_data.h
)

if(BUILD_FOR_RASPBERRY_PI_PICO)
	target_link_libraries(${BinName} pico_stdlib hardware_i2c)
	pico_add_extra_outputs(${BinName})
endif()
