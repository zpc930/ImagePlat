#pragma once
#pragma once
#include <opencv2\opencv.hpp>
#include "CommonHeader.h"

class SelectiveBlur
{
public:
	SelectiveBlur();
	~SelectiveBlur();
	static void Calc(unsigned short *Hist, int Intensity, unsigned char *&Pixel, int Threshold);
	static int Process(cv::Mat &image, int Radius, int Threshold, EdgeMode Edge);
	static int BlurChannel(cv::Mat &Src, int Radius, int Threshold, EdgeMode Edge);
};

