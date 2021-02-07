# Bad Apple!! on OLED
- Play "Bad Apple!!" on OLED
	- on Raspberry Pi
	- on Raspberry Pi Pico

## Hardware
- OLED
	- SH1106 (I2C)
	- 128x64

## on Raspberry Pi 4
### Connection
- i2c1
	- GPIO2(SDA)
	- GPIO3(SCL)
	- 3V3
	- GND

### Build
```
cmake .. -DBUILD_FOR_RASPBERRY_PI_PICO=off
make
```

## on Raspberry Pi Pico
### Connection
- i2c0
	- GP0(SDA, 1)
	- GP1(SCL, 2)
	- 3V3(36)
	- GND(38)

### Build
```
cmake .. -G "NMake Makefiles" -DBUILD_FOR_RASPBERRY_PI_PICO=on
nmake
```

## Convert image data
- `tool_converter` project converts image file(jpeg/mp4) to bin and C array
- copy generated C array code and paste to `VideoData.h`

