/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

#include <fstream>
#include <iostream>
#include <iterator>

/* for OpenCV */
#include <opencv2/opencv.hpp>

/*** Macro ***/
/* Settings */
static const int32_t WIDTH = 128;
static const int32_t HEIGHT = 64;
static const int32_t PAGE_SIZE = 8;
static const int32_t PAGE_NUM = 8;

static const uint8_t THRESHOLD_BLIGHTNESS = 128;

static const int32_t GRID_NUM_X = 5;
static const int32_t GRID_NUM_Y = 10;
static const int32_t ADJUST_PX_X = 30;
static const int32_t ADJUST_PX_Y = 30;

int32_t main()
{
	std::string filename = "pictogram.jpg";

	cv::Mat resultMat = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC1);
	std::ofstream ofs("out.bin", std::ios::out | std::ios::binary);
	std::ofstream ofsCode("VideoData.h", std::ios::out);
	ofsCode << std::showbase;
	ofsCode << "const unsigned char VideoData[][128*8] = { \n";
	
	cv::Mat original_image = cv::imread(filename);
	int32_t grid_size_width = original_image.cols / GRID_NUM_X;
	int32_t grid_size_height = original_image.rows / GRID_NUM_Y;
	for (int32_t y = 0; y < GRID_NUM_Y; y++) {
		for (int32_t x = 0; x < GRID_NUM_X; x++) {
			cv::Rect crop(x * grid_size_width + ADJUST_PX_X, y * grid_size_height + ADJUST_PX_Y, grid_size_width - ADJUST_PX_X * 1.5, grid_size_height - ADJUST_PX_Y * 1.5);
			cv::Mat inputMat = original_image(crop);
			//cv::imshow("a", inputMat);
			//cv::waitKey(-1);
			//continue;

			cv::cvtColor(inputMat, inputMat, cv::COLOR_BGR2GRAY);
			inputMat = 255 - inputMat;	// reverse black and white

			double scale = (std::min)(static_cast<double>(WIDTH) / inputMat.cols, static_cast<double>(HEIGHT) / inputMat.rows);
			cv::resize(inputMat, inputMat, cv::Size(), scale, scale);
			inputMat.copyTo(resultMat(cv::Rect((resultMat.cols - inputMat.cols) / 2, (resultMat.rows - inputMat.rows) / 2, inputMat.cols, inputMat.rows)));
			cv::imshow("resultMat", resultMat);
			if (cv::waitKey(1) == 'q') break;

			std::vector<uint8_t> resultData;
			for (int32_t page = 0; page < PAGE_NUM; page++) {
				for (int32_t x = 0; x < WIDTH; x++) {
					uint8_t pageData = 0;
					for (int32_t i = 0; i < PAGE_SIZE; i++) {
						int32_t index = (page * PAGE_SIZE + i) * WIDTH + x;
						uint8_t onoff = (resultMat.data[index] > THRESHOLD_BLIGHTNESS) ? 1 : 0;
						pageData |= onoff << i;
					}
					resultData.push_back(pageData);
				}
			}
			(void)ofs.write(reinterpret_cast<char*>(resultData.data()), resultData.size());

			ofsCode << "{ ";
			for (const auto data : resultData) {
				ofsCode << std::hex << static_cast<int32_t>(data) << ", ";
			}
			ofsCode << " },\n";

			if (inputMat.empty()) break;
		}
	}

	ofsCode << "};\n";
	ofsCode.close();
	ofs.close();

	return 0;
}
