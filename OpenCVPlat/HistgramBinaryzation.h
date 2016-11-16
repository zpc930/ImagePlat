#pragma once
#include <opencv2\opencv.hpp>
class HistgramBinaryzation
{
public:
	HistgramBinaryzation();
	~HistgramBinaryzation();
	static int GetMeanThreshold(int* HistGram,int nSzie);
	static int GetPTileThreshold(int* HistGram, int Tile = 50, int nSize=256);
	static int GetHuangFuzzyThreshold(int* HistGram, int nSize);
	static int WellneradaptiveThreshold1(cv::Mat &image, int Radius = 5, int Threshold = 15);
	static int WellneradaptiveThreshold2(cv::Mat &image, int Radius = 5, int Threshold = 50);
	static int adaptiveThreshold(cv::Mat &image);
	static int HuangFuzzyThreshold(cv::Mat &image);
	static  void computeHistogram(cv::Mat &input, int *hist);
};

