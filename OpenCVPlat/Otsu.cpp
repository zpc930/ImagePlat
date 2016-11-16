#include "stdafx.h"
#include "Otsu.h"

#define  MAX_INTENSITY 255

Otsu::Otsu()
{
}


Otsu::~Otsu()
{
}
void computeHistogram(cv::Mat &input, unsigned *hist)
{
	int Width = input.cols;
	int Height = input.rows;
	for (int i = 0; i <= MAX_INTENSITY; i++) hist[i] = 0;
	for (int Y = 0; Y < Height; Y++)
	{
		unsigned char* LinePD = input.ptr<uchar>(Y);
		for (int X = 0; X < Width  ; X++)
		{		
			int value = (int)LinePD[X];
				hist[value]++;			
		}
	}
}
void segmentImage(cv::Mat &input, int threshold) {
	// Compute number of pixels
	long int N = input.cols * input.rows;
	// Modify output image
	// Use first channel
	int Width = input.cols;
	int Height = input.rows;
	// Iterate image
	for (int Y = 0; Y < Height; Y++)
	{
		unsigned char* LinePD = input.ptr<uchar>(Y);
		for (int X = 0; X < Width; X++)
		{
			int value = (int)LinePD[X];
			if (value > threshold){
				LinePD[X] = 255.0f;
			}
			else{
				LinePD[X] = 0.0f;
			}
		}
	}

}
void computeOtsusSegmentation(cv::Mat &input, unsigned* hist, int overrided_threshold)
{
	int threshold = 0;
	long int N = input.cols * input.rows;
	if (overrided_threshold != 0){
		// If threshold was manually entered
		threshold = overrided_threshold;
	}
	else
	{
		float sum = 0;
		float sumB = 0;
		int q1 = 0;
		int q2 = 0;
		float varMax = 0;

		// Auxiliary value for computing m2
		for (int i = 0; i <= MAX_INTENSITY; i++){
			sum += i * ((int)hist[i]);
		}
		for (int i = 0; i <= MAX_INTENSITY; i++) {
			// Update q1
			q1 += hist[i];
			if (q1 == 0)
				continue;
			// Update q2
			q2 = N - q1;

			if (q2 == 0)
				break;
			// Update m1 and m2
			sumB += (float)(i * ((int)hist[i]));
			float m1 = sumB / q1;
			float m2 = (sum - sumB) / q2;

			// Update the between class variance
			float varBetween = (float)q1 * (float)q2 * (m1 - m2) * (m1 - m2);

			// Update the threshold if necessary
			if (varBetween > varMax) {
				varMax = varBetween;
				threshold = i;
			}
		}
	}
	segmentImage(input,  threshold);
}

int Otsu::Process(cv::Mat &image)
{
	int Width = image.cols;
	int Height = image.rows;
	cv::Mat imagegray;
	imagegray.create(Height, Width, CV_8UC1);
	cvtColor(image, imagegray, CV_BGR2GRAY);
	unsigned hist[MAX_INTENSITY + 1];
	computeHistogram(imagegray, hist);
	computeOtsusSegmentation(imagegray, hist,0);
	image = imagegray;
	return 0;
}