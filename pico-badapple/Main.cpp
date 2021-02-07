#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "OledSh1106.h"
#include "VideoData.h"

int main()
{
	OledSh1106 oled;
	oled.initialize();
	oled.fillRect(0, 0, 0, OledSh1106::WIDTH, OledSh1106::HEIGHT);
	oled.setCharPos(8, 4);
	oled.printText("Bad Apple!!");

	int c = getchar();

	for (const auto buffer : VideoData) {
		oled.sendImage(buffer);
	}

	return 0;
}

