#pragma once
#include <opencv2\opencv.hpp>
class MoravecFP
{
public:
	MoravecFP();
	~MoravecFP();
	static int MaravecProcess(cv::Mat& f_image);
};

