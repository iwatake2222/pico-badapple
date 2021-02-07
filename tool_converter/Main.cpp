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

int32_t main()
{
	std::string filename = "smile.mp4";
	cv::VideoCapture cap(filename);

	cv::Mat resultMat = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC1);
	std::ofstream ofs("out.bin", std::ios::out | std::ios::binary);
	std::ofstream ofsCode("VideoData.h", std::ios::out);
	ofsCode << std::showbase;
	ofsCode << "const unsigned char VideoData[][128*8] = { \n";
	
	while (1) {
		cv::Mat inputMat;
		if (cap.isOpened()) {
			cap.read(inputMat);
		} else {
			inputMat = cv::imread(filename);
		}
		if (inputMat.empty()) break;


		cv::cvtColor(inputMat, inputMat, cv::COLOR_BGR2GRAY);
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

	ofsCode << "};\n";
	ofsCode.close();
	ofs.close();

	return 0;
}
