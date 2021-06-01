set(BinName "pico-badapple")
add_executable(${BinName}
	Main.cpp
	oled.cpp
	oled.h
	font.cpp
	VideoData.h
)

if(BUILD_FOR_RASPBERRY_PI_PICO)
	target_link_libraries(${BinName} pico_stdlib hardware_i2c)
	pico_add_extra_outputs(${BinName})
endif()
