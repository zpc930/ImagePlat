#pragma once
#include <opencv2\opencv.hpp>
enum WB_PRESET{
	//自动白平衡
	AUTO,
	//阴天 7500k
	CLOUDY,
	//日光 6500k
	DAYLIGHT,
	//白热光 5000k
	INCANDESCENCE,
	//日光灯 4400k
	FLUORESCENT,
	//钨丝灯 2800k
	TUNGSTEN,
};
class GrayWorld
{
public:
	GrayWorld();
	~GrayWorld();
	static int Process(cv::Mat& f_image);
	static int perfectReflector(cv::Mat& f_image);
	static int colorbalance_rgb_u8(cv::Mat& f_image);
};

