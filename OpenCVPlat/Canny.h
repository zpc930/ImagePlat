#pragma once
#include <opencv2\opencv.hpp>
class Canny
{
public:
	Canny();
	~Canny();
	static int CannyProcess(cv::Mat& f_image);
private:
	static  int GaussBlur(cv::Mat& grayImage,double nSigma);
};

