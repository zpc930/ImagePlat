#pragma once
#include <opencv2\opencv.hpp>
class Otsu
{
public:
	Otsu();
	~Otsu();
	static int Process(cv::Mat &image);
};

