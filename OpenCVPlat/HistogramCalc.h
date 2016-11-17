#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
typedef _Longlong int64_t;
typedef int64_t int64;
typedef signed __int32 int32_t;
typedef unsigned char uint8_t;

class CHistogramCalc
{
public:
	struct SHistMap
	{
		std::vector<double> m_xvalues;
		std::vector<double> m_yvalues;
	};

public:
	CHistogramCalc();
	~CHistogramCalc();
	void calculateHistogram(const cv::Mat &f_image);
	int32_t Histogram(const cv::Mat& f_image, int32_t* red, int32_t* green, int32_t* blue, int32_t* gray, int32_t colorspace);
	int32_t InteEqualize(cv::Mat& f_image);
	int32_t AHE(cv::Mat& f_image,int nNumber);
private:

	SHistMap m_channel0;
	SHistMap m_channel1;
	SHistMap m_channel2;

	int64 m_pixelCount;
};

