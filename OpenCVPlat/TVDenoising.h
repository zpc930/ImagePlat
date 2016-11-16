#pragma once
#include "CommonHeader.h"
#include <opencv2\opencv.hpp>
class TVDenoising
{
public:
	TVDenoising();
	~TVDenoising();
	static int ProcessTVD(cv::Mat &image, int iter);
	static double** newDoubleMatrix(int nx, int ny);
	static bool deleteDoubleMatrix(double** matrix, int nx, int ny);
};

