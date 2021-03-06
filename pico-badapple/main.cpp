#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "oled.h"
#include "video_data.h"

#if defined(PLATFORM_PIPICO)
#include "pico/stdlib.h"
#endif

int main()
{
	Oled oled;
	oled.Initialize();
	oled.FillRect(0, 0, 0, Oled::kWidth, Oled::kHeight);
	oled.SetCharPos(8, 4);
	oled.PrintText("Bad Apple!!");
	
#if defined(PLATFORM_LINUX)
	int c = getchar();
#elif defined(PLATFORM_PIPICO)
	sleep_ms(1000);
#endif

	while(1) {
		for (const auto buffer : VideoData) {
			oled.SendImage(buffer);
		}
	}

	return 0;
}

