#ifndef OLED_H_
#define OLED_H_

#include <cstdint>

// The following is defined in cmake
// #define PLATFORM_LINUX
#define PLATFORM_PIPICO

class Oled
{
public:
	/* Fixed config for device */
	static constexpr bool    kIsSh1106 = true;
	static constexpr uint8_t kSlaveAddress = 0x3C;
	static constexpr uint8_t kWidth  = 128;
	static constexpr uint8_t kHeight = 64;
	static constexpr uint8_t kPageSize = 8;
	static constexpr uint8_t kPageNum = 8;

	/* config for printing text */
	static constexpr uint8_t kTextAreaOriginX = 0;
	static constexpr uint8_t kTextAreaOriginY = 0;
	static constexpr uint8_t kTextAreaEndX = 128;
	static constexpr uint8_t kTextAreaEndY = 64;
	static constexpr uint8_t kFontWidth = 5;
	static constexpr uint8_t kFontHeight = 8;

private:
#if defined(PLATFORM_LINUX)
	const char *I2C_DEV_NAME = "/dev/i2c-1";
	int32_t fd_;
#elif defined(PLATFORM_PIPICO)
	static constexpr uint8_t kGpioNumSda = 18;	// GP18
	static constexpr uint8_t kGpioNumScl = 19;	// GP19
	static constexpr int32_t kI2cNum = 1;		// i2c1
#endif

private:
	int32_t I2cInitialize(uint8_t slaveAddress);
	void I2cFinalize();
	void I2cWrite(uint8_t* buffer, int32_t len);
	void I2cRead(uint8_t* buffer, int32_t len);

public:
	void SendCommand(uint8_t cmd);
	void SendData(uint8_t data);
	void DrawChar(char c);

public:
	Oled();
	~Oled();
	int32_t Initialize();
	void Finalize();
	void SendImage(const uint8_t* buffer);
	void SetPos(uint8_t x, uint8_t y);
	void FillRect(uint8_t data, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void SetCharPos(uint8_t textX, uint8_t textY);
	void PutChar(char c);
	void PrintText(const char* text);

private:
	// x, y is pixel
	uint8_t current_pos_x_;
	uint8_t current_pos_y_;

};

#endif /* OLED_H_ */
