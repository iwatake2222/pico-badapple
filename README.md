# Bad Apple!! on OLED
- Play "Bad Apple!!" on OLED
	- on Raspberry Pi
	- on Raspberry Pi Pico

## Hardware
- OLED
	- SH1106 (I2C)
	- 128x64
- Raspberry Pi
	- i2c1
		- GPIO2(SDA)
		- GPIO3(SCL)
		- 3V3
		- GND
- Raspberry Pi Pico
	- todo

## Setting
- Platform (enable either of the followings)
	- `#define PLATFORM_LINUX`
	- `#define PLATFORM_PIPICO`

## Prepare image data
- `tool_converter` project converts image file(jpeg/mp4) to bin and C array
- copy generated C array code and paste to `VideoData.h`
