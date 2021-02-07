#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "OledSh1106.h"
#include "VideoData.h"

#if defined(PLATFORM_PIPICO)
#include "pico/stdlib.h"
#endif

int main()
{
	OledSh1106 oled;
	oled.initialize();
	oled.fillRect(0, 0, 0, OledSh1106::WIDTH, OledSh1106::HEIGHT);
	oled.setCharPos(8, 4);
	oled.printText("Bad Apple!!");
	
#if defined(PLATFORM_LINUX)
	int c = getchar();
#elif defined(PLATFORM_PIPICO)
	sleep_ms(1000);
#endif

	for (const auto buffer : VideoData) {
		oled.sendImage(buffer);
	}

	return 0;
}

