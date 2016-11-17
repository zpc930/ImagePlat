#pragma once
#include <opencv2\opencv.hpp>
class bilateralFilter
{
public:
	bilateralFilter();
	~bilateralFilter();
	static void Process(cv::Mat& f_image);
	static void bilateralFilter::bilateralFilterProcess(unsigned char* pSrc, unsigned char* pDest, int width, int height, int radius);
};

