#pragma once
#include <opencv2\opencv.hpp>
class FindEdges
{
public:
	FindEdges();
	~FindEdges();
	static int mdFindEdges(cv::Mat &image);
	static int SalientRegionDetectionBasedonLC(cv::Mat &Src);
};

