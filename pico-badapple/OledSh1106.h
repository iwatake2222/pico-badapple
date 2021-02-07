#ifndef OLED_SH_1106_H_
#define OLED_SH_1106_H_

#include <cstdint>

#define PLATFORM_LINUX
// #define PLATFORM_PIPICO

class OledSh1106
{
public:
	/* Fixed config by device */
	static constexpr uint8_t SLAVE_ADDRESS = 0x3C;
	static constexpr uint8_t WIDTH  = 128;
	static constexpr uint8_t HEIGHT = 64;
	static constexpr uint8_t PAGE_SIZE = 8;
	static constexpr uint8_t PAGE_NUM = 8;

	/* config for printing text */
	static constexpr uint8_t TEXT_AREA_ORIGIN_X = 0;
	static constexpr uint8_t TEXT_AREA_ORIGIN_Y = 0;
	static constexpr uint8_t TEXT_AREA_END_X = 128;
	static constexpr uint8_t TEXT_AREA_END_Y = 64;
	static constexpr uint8_t FONT_WIDTH = 5;
	static constexpr uint8_t FONT_HEIGHT = 8;

private:
	// x, y is pixel
	uint8_t m_currentPosX;
	uint8_t m_currentPosY;

private:
#if defined(PLATFORM_LINUX)
	const char *I2C_DEV_NAME = "/dev/i2c-1";
	int32_t m_fd;
#elif defined(PLATFORM_PIPICO)
#endif

private:
	int32_t i2cInitialize(uint8_t slaveAddress);
	void i2cFinalize();
	void i2cWrite(uint8_t* buffer, int32_t len);
	void i2cRead(uint8_t* buffer, int32_t len);

public:
	void sendCommand(uint8_t cmd);
	void sendData(uint8_t data);
	void drawChar(char c);

public:
	OledSh1106();
	~OledSh1106();
	int32_t initialize();
	void finalize();
	void sendImage(uint8_t* buffer);
	void setPos(uint8_t x, uint8_t y);
	void fillRect(uint8_t data, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void setCharPos(uint8_t textX, uint8_t textY);
	void putChar(char c);
	void printText(const char* text);
};

#endif /* OLED_SH_1106_H_ */
