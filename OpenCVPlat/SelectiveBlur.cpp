#include "stdafx.h"
#include "SelectiveBlur.h"


SelectiveBlur::SelectiveBlur()
{
}


SelectiveBlur::~SelectiveBlur()
{
}
void SelectiveBlur::Calc(unsigned short *Hist, int Intensity, unsigned char *&Pixel, int Threshold)
{
	int K, Low, High, Sum = 0, Weight = 0;
	Low = Intensity - Threshold; High = Intensity + Threshold;
	if (Low < 0) Low = 0;
	if (High > 255) High = 255;
	for (K = Low; K <= High; K++)
	{
		Weight += Hist[K];
		Sum += Hist[K] * K;
	}
	if (Weight != 0) *Pixel = Sum / Weight;
}
int SelectiveBlur::BlurChannel(cv::Mat &Src, int Radius, int Threshold, EdgeMode Edge)
{
	int X = 0;
	int Y = 0;
	int K = 0;
	int Width = Src.cols;
	int Height = Src.rows;
	unsigned short *ColHist = (unsigned short *)malloc(256 * (Width + 2 * Radius) * sizeof(unsigned short));
	unsigned short *Hist = (unsigned short *)malloc(256 * sizeof(unsigned short));

	return 0;
}
int SelectiveBlur::Process(cv::Mat &image, int Radius, int Threshold, EdgeMode Edge)
{
	return 0;
}