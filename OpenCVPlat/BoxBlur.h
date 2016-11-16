#pragma once
#include <opencv2\opencv.hpp>
#include "CommonHeader.h"

class BoxBlur
{
public:
	BoxBlur();
	~BoxBlur();
	static int Process(cv::Mat &image, int Radius, EdgeMode Edge);
	static int Hilditch(cv::Mat &image);
};

