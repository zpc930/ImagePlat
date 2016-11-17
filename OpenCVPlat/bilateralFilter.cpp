#include "stdafx.h"
#include "bilateralFilter.h"


bilateralFilter::bilateralFilter()
{
}


bilateralFilter::~bilateralFilter()
{
}
 void bilateralFilter::Process(cv::Mat& f_image)
{
	std::vector<cv::Mat> channels;
	cv::split(f_image, channels);
	int width = f_image.cols;
	int height = f_image.rows;
	int size = width*height;	
	cv::Mat m[3];
	m[0] = channels[0];
	m[1] = channels[1];
	m[2] = channels[2];
	bilateralFilterProcess(m[0].data, channels[0].data, width, height, 5);
	bilateralFilterProcess(m[1].data, channels[1].data, width, height, 5);
	bilateralFilterProcess(m[2].data, channels[2].data, width, height, 5);
	cv::merge(channels, f_image);
}
void bilateralFilter::bilateralFilterProcess(unsigned char* pSrc, unsigned char* pDest, int width, int height, int radius)
{
	float delta = 0.1f;
	float eDelta = 1.0f / (2 * delta * delta);

	int colorDistTable[256 * 256] = { 0 };
	for (int x = 0; x < 256; x++)
	{
		int  * colorDistTablePtr = colorDistTable + (x * 256);
		for (int y = 0; y < 256; y++)
		{
			int  mod = ((x - y) * (x - y))*(1.0f / 256.0f);
			colorDistTablePtr[y] = 256 * exp(-mod * eDelta);
		}
	}
	for (int Y = 0; Y < height; Y++)
	{
		int Py = Y * width;
		unsigned char* LinePD = pDest + Py;
		unsigned char* LinePS = pSrc + Py;
		for (int X = 0; X < width; X++)
		{
			int sumPix = 0;
			int sum = 0;
			int factor = 0;

			for (int i = -radius; i <= radius; i++)
			{
				unsigned char* pLine = pSrc + ((Y + i + height) % height)* width;
				int cPix = 0;
				int  * colorDistPtr = colorDistTable + LinePS[X] * 256;
				for (int j = -radius; j <= radius; j++)
				{
					cPix = pLine[(X + j + width) % width];
					factor = colorDistPtr[cPix];
					sum += factor;
					sumPix += (factor *cPix);
				}
			}
			LinePD[X] = (sumPix / sum);
		}
	}
}