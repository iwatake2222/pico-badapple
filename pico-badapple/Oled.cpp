#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "oled.h"
#include "oled_cmd.h"

#if defined(PLATFORM_LINUX)
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#elif defined(PLATFORM_PIPICO)
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#endif

Oled::Oled()
{
}

Oled::~Oled()
{
}

int32_t Oled::Initialize()
{
	I2cInitialize(kSlaveAddress);

	/* init sequence */
	SendCommand(SH1106_DISPLAYOFF);
	SendCommand(SH1106_SETMULTIPLEX);
	SendCommand(0x3F);
	SendCommand(SH1106_SETDISPLAYOFFSET);
	SendCommand(0);
	SendCommand(SH1106_SETSTARTLINE | 0x00);
	SendCommand(SH1106_SEGREMAP | 0x01);
	SendCommand(SH1106_COMSCANDEC);
	SendCommand(SH1106_SETCOMPINS);
	SendCommand(0x12);
	SendCommand(SH1106_SETCONTRAST);
	SendCommand(0xCF);
	SendCommand(SH1106_DISPLAYALLON_RESUME);
	SendCommand(SH1106_NORMALDISPLAY);
	SendCommand(SH1106_SETDISPLAYCLOCKDIV);
	SendCommand(0x80);
	SendCommand(SH1106_CHARGEPUMP);
	SendCommand(0x14);
	SendCommand(SH1106_MEMORYMODE);
	SendCommand(0x00);  // vertical addressing mode
	SendCommand(0x02);  /* column address = 2 */
	SendCommand(0x10);
	SendCommand(0x40);  /* display start line = 0 */
	SendCommand(0xB0);  /* page start = 0 */
	SendCommand(SH1106_DISPLAYON);

	FillRect(1, 0, 0, kWidth, kHeight);
	SetPos(0, 0);
	current_pos_x_ = kTextAreaOriginX;
	current_pos_y_ = kTextAreaOriginY;
	return 0;
}

void Oled::Finalize()
{
	I2cFinalize();
}

// y must be 8 align
void Oled::SetPos(uint8_t x, uint8_t y)
{
	if (kIsSh1106) {
		x += 2; /* column pos offset of 2 */
	}
	SendCommand(0x10 | (uint8_t)(x >> 4)); /* higher column address */
	SendCommand(x & 0x0F);          /* lower column address */
	SendCommand(0xB0 + (y / kPageSize));  /* row address */
}

// y must be 8 align
void Oled::FillRect(uint8_t on, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t data = (on == 0 ? 0 : 0xff);

	for(uint8_t y = y0; y < y1; y += kPageSize) {
		SetPos(x0, y);
		for(uint8_t x = x0; x < x1; x++) {
			SendData(data);
		}
	}
}

void Oled::SetCharPos(uint8_t textX, uint8_t textY)
{
	current_pos_x_ = kTextAreaOriginX + textX * kFontWidth;
	current_pos_y_ = kTextAreaOriginY + textY * kFontHeight;
	if( current_pos_x_ > kTextAreaEndX ) {
		current_pos_x_ = kTextAreaOriginX;
	}
	if( current_pos_y_ > kTextAreaEndY ) {
		current_pos_y_ = kTextAreaOriginY;
	}
}

void Oled::PutChar(char c)
{
	uint8_t end_x = current_pos_x_ + kFontWidth;
	uint8_t end_y = current_pos_y_ + kFontHeight;
	if(c == '\r' || c == '\n') {
	  end_x = kTextAreaEndX + 1;
	}
	if( end_x > kTextAreaEndX ) {
		current_pos_x_ = kTextAreaOriginX;
		end_x = current_pos_x_ + kFontWidth;
		current_pos_y_ += kFontHeight;
		end_y = current_pos_y_ + kFontHeight;
		if( end_y > kTextAreaEndY ) {
			current_pos_y_ = kTextAreaOriginY;
			end_y = current_pos_y_ + kFontHeight;
		}
		FillRect(0, kTextAreaOriginX, current_pos_y_, kTextAreaEndX, end_y);
	}

	SetPos(current_pos_x_, current_pos_y_);
	if(c == '\r' || c == '\n') {
		return;
	}
	DrawChar(c);

	current_pos_x_ = end_x;
}

void Oled::PrintText(const char* text)
{
	while(*text != '\0') {
		PutChar(*text);
		text++;
	}
}

void Oled::DrawChar(char c)
{
	extern uint8_t font[];
	for (uint8_t i = 0; i < kFontWidth; i++ ) {	
		uint8_t line = *(font + (c * kFontWidth) + i);
		SendData(line);
	}
}


void Oled::SendImage(const uint8_t* buffer)
{
	uint8_t* txBuffer = new uint8_t[1 + kWidth];
	txBuffer[0] = CONTROL_DATA;
	for (int32_t page = 0; page < kPageNum; page++) {
		SetPos(0, page * kPageSize);
		memcpy(txBuffer + 1, buffer, kWidth);
		buffer += kWidth;
		I2cWrite(txBuffer, 1 + kWidth);
	}
	delete[] txBuffer;
}

void Oled::SendData(uint8_t data)
{
	uint8_t buffer[2];
	buffer[0] = CONTROL_DATA;
	buffer[1] = data;
	I2cWrite(buffer, 2);
}

void Oled::SendCommand(uint8_t cmd)
{
	uint8_t buffer[2];
	buffer[0] = CONTROL_COMMAND;
	buffer[1] = cmd;
	I2cWrite(buffer, 2);
}


int32_t Oled::I2cInitialize(uint8_t slaveAddress)
{
#if defined(PLATFORM_LINUX)
	if((fd_ = open(I2C_DEV_NAME, O_RDWR)) < 0){
		printf("i2c error\n");
		return -1;
	}
	if (ioctl(fd_, I2C_SLAVE, slaveAddress) < 0) {
		printf("i2c error\n");
		return -1;
	}
#elif defined(PLATFORM_PIPICO)
	i2c_init(i2c0, 2000 * 1000);	/* 2MHz (might not work correctly) */
	gpio_set_function(kGpioNumSda, GPIO_FUNC_I2C);
	gpio_set_function(kGpioNumScl, GPIO_FUNC_I2C);
	gpio_pull_up(kGpioNumSda);
	gpio_pull_up(kGpioNumScl);
#endif
	return 0;
}


void Oled::I2cFinalize()
{
#if defined(PLATFORM_LINUX)
	close(fd_);
#elif defined(PLATFORM_PIPICO)
#endif
}

void Oled::I2cWrite(uint8_t* buffer, int32_t len)
{
#if defined(PLATFORM_LINUX)
	if (write(fd_, buffer, len) != len) {
		printf("[error] I2cWrite\n");
	}
#elif defined(PLATFORM_PIPICO)
	i2c_write_blocking(i2c0, kSlaveAddress, buffer, len, false);
#endif
}

void Oled::I2cRead(uint8_t* buffer, int32_t len)
{
#if defined(PLATFORM_LINUX)
	if (read(fd_, buffer, len) != len) {
		printf("[error] I2cRead\n");
	}
#elif defined(PLATFORM_PIPICO)
	i2c_read_blocking(i2c0, kSlaveAddress, buffer, len, false);
#endif
}
