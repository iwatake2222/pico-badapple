#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "OledSh1106.h"
#include "OledSh1106Cmd.h"

#if defined(PLATFORM_LINUX)
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#elif defined(PLATFORM_PIPICO)
// todo
#endif

OledSh1106::OledSh1106()
{
}

OledSh1106::~OledSh1106()
{
}

int32_t OledSh1106::initialize()
{
	i2cInitialize(SLAVE_ADDRESS);

	/* init sequence */
	sendCommand(SH1106_DISPLAYOFF);
	sendCommand(SH1106_SETMULTIPLEX);
	sendCommand(0x3F);
	sendCommand(SH1106_SETDISPLAYOFFSET);
	sendCommand(0);
	sendCommand(SH1106_SETSTARTLINE | 0x00);
	sendCommand(SH1106_SEGREMAP | 0x01);
	sendCommand(SH1106_COMSCANDEC);
	sendCommand(SH1106_SETCOMPINS);
	sendCommand(0x12);
	sendCommand(SH1106_SETCONTRAST);
	sendCommand(0xCF);
	sendCommand(SH1106_DISPLAYALLON_RESUME);
	sendCommand(SH1106_NORMALDISPLAY);
	sendCommand(SH1106_SETDISPLAYCLOCKDIV);
	sendCommand(0x80);
	sendCommand(SH1106_CHARGEPUMP);
	sendCommand(0x14);
	sendCommand(SH1106_MEMORYMODE);
	sendCommand(0x00);  // vertical addressing mode
	sendCommand(0x02);  /* column address = 2 */
	sendCommand(0x10);
	sendCommand(0x40);  /* display start line = 0 */
	sendCommand(0xB0);  /* page start = 0 */
	sendCommand(SH1106_DISPLAYON);

	fillRect(1, 0, 0, WIDTH, HEIGHT);
	setPos(0, 0);
	m_currentPosX = TEXT_AREA_ORIGIN_X;
	m_currentPosY = TEXT_AREA_ORIGIN_Y;
	return 0;
}

void OledSh1106::finalize()
{
	i2cFinalize();
}

// y must be 8 align
void OledSh1106::setPos(uint8_t x, uint8_t y)
{
	x += 2; /* column pos offset of 2 */
	sendCommand(0x10 | (uint8_t)(x >> 4)); /* higher column address */
	sendCommand(x & 0x0F);          /* lower column address */
	sendCommand(0xB0 + (y / PAGE_SIZE));  /* row address */
}

// y must be 8 align
void OledSh1106::fillRect(uint8_t on, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t data = (on == 0 ? 0 : 0xff);

	for(uint8_t y = y0; y < y1; y += PAGE_SIZE) {
		setPos(x0, y);
		for(uint8_t x = x0; x < x1; x++) {
			sendData(data);
		}
	}
}

void OledSh1106::setCharPos(uint8_t textX, uint8_t textY)
{
	m_currentPosX = TEXT_AREA_ORIGIN_X + textX * FONT_WIDTH;
	m_currentPosY = TEXT_AREA_ORIGIN_Y + textY * FONT_HEIGHT;
	if( m_currentPosX > TEXT_AREA_END_X ) {
		m_currentPosX = TEXT_AREA_ORIGIN_X;
	}
	if( m_currentPosY > TEXT_AREA_END_Y ) {
		m_currentPosY = TEXT_AREA_ORIGIN_Y;
	}
}

void OledSh1106::putChar(char c)
{
	uint8_t endX = m_currentPosX + FONT_WIDTH;
	uint8_t endY = m_currentPosY + FONT_HEIGHT;
	if(c == '\r' || c == '\n') {
	  endX = TEXT_AREA_END_X + 1;
	}
	if( endX > TEXT_AREA_END_X ) {
		m_currentPosX = TEXT_AREA_ORIGIN_X;
		endX = m_currentPosX + FONT_WIDTH;
		m_currentPosY += FONT_HEIGHT;
		endY = m_currentPosY + FONT_HEIGHT;
		if( endY > TEXT_AREA_END_Y ) {
			m_currentPosY = TEXT_AREA_ORIGIN_Y;
			endY = m_currentPosY + FONT_HEIGHT;
		}
		fillRect(0, TEXT_AREA_ORIGIN_X, m_currentPosY, TEXT_AREA_END_X, endY);
	}

	setPos(m_currentPosX, m_currentPosY);
	if(c == '\r' || c == '\n') {
		return;
	}
	drawChar(c);

	m_currentPosX = endX;
}

void OledSh1106::printText(const char* text)
{
	while(*text != '\0') {
		putChar(*text);
		text++;
	}
}

void OledSh1106::drawChar(char c)
{
	extern uint8_t font[];
	for (uint8_t i = 0; i < FONT_WIDTH; i++ ) {	
		uint8_t line = *(font + (c * FONT_WIDTH) + i);
		sendData(line);
	}
}


void OledSh1106::sendImage(uint8_t* buffer)
{
	uint8_t* txBuffer = new uint8_t[1 + WIDTH];
	txBuffer[0] = CONTROL_DATA;
	for (int32_t page = 0; page < PAGE_NUM; page++) {
		setPos(0, page * PAGE_SIZE);
		memcpy(txBuffer + 1, buffer, WIDTH);
		buffer += WIDTH;
		i2cWrite(txBuffer, 1 + WIDTH);
	}
	delete[] txBuffer;
}

void OledSh1106::sendData(uint8_t data)
{
	uint8_t buffer[2];
	buffer[0] = CONTROL_DATA;
	buffer[1] = data;
	i2cWrite(buffer, 2);
}

void OledSh1106::sendCommand(uint8_t cmd)
{
	uint8_t buffer[2];
	buffer[0] = CONTROL_COMMAND;
	buffer[1] = cmd;
	i2cWrite(buffer, 2);
}


int32_t OledSh1106::i2cInitialize(uint8_t slaveAddress)
{
#if defined(PLATFORM_LINUX)
	if((m_fd = open(I2C_DEV_NAME, O_RDWR)) < 0){
		printf("i2c error\n");
		return -1;
	}
	if (ioctl(m_fd, I2C_SLAVE, slaveAddress) < 0) {
		printf("i2c error\n");
		return -1;
	}
#elif defined(PLATFORM_PIPICO)
#endif
	return 0;
}


void OledSh1106::i2cFinalize()
{
#if defined(PLATFORM_LINUX)
	close(m_fd);
#elif defined(PLATFORM_PIPICO)
#endif
}

void OledSh1106::i2cWrite(uint8_t* buffer, int32_t len)
{
#if defined(PLATFORM_LINUX)
	if (write(m_fd, buffer, len) != len) {
		printf("[error] i2cWrite\n");
	}
#elif defined(PLATFORM_PIPICO)
#endif
}

void OledSh1106::i2cRead(uint8_t* buffer, int32_t len)
{
#if defined(PLATFORM_LINUX)
	if (read(m_fd, buffer, len) != len) {
		printf("[error] i2cRead\n");
	}
#elif defined(PLATFORM_PIPICO)
#endif
}
