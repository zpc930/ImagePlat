#include "stdafx.h"
#include "GrayWorld.h"
#define min(a,b) (((a) < (b)) ? (a) : (b))

GrayWorld::GrayWorld()
{
}


GrayWorld::~GrayWorld()
{
}
int GrayWorld::Process(cv::Mat& f_image)
{
	double sumR = 0, sumG = 0, sumB = 0;
	int height = f_image.size().height;
	int width = f_image.size().width;
	for (int y = 0; y < height;y++)
	{
		uchar *data = f_image.ptr<uchar>(y);

		for (int x = 0; x < width;x++)
		{
			int bn = data[x * 3];
			int gn = data[x * 3 + 1] ;
			int rn = data[x * 3 + 2];
			sumR += rn;
			sumB += bn;
			sumG += gn;
		}
	}
	float Raver, Gaver, Baver;
	Raver = sumR/(height*width);
	Gaver = sumG / (height*width);
	Baver = sumB / (height*width);
	int numberOfPixels = height*width;
	int preset = CLOUDY;
	switch (preset)
	{
	case AUTO:
		Raver = (sumR / numberOfPixels);
		Gaver = (sumG / numberOfPixels);
		Baver = (sumB / numberOfPixels);
		break;
	case CLOUDY:
		Raver = (sumR *1.953125 / numberOfPixels);
		Gaver = (sumG*1.0390625 / numberOfPixels);
		Baver = (sumB / numberOfPixels);
		break;
	case DAYLIGHT:
		Raver = (sumR *1.2734375 / numberOfPixels);
		Gaver = (sumG / numberOfPixels);
		Baver = (sumB*1.0625 / numberOfPixels);
		break;
	case INCANDESCENCE:
		Raver = (sumR *1.2890625 / numberOfPixels);
		Gaver = (sumG / numberOfPixels);
		Baver = (sumB*1.0625 / numberOfPixels);
		break;
	case FLUORESCENT:
		Raver = (sumR *1.1875 / numberOfPixels);
		Gaver = (sumG / numberOfPixels);
		Baver = (sumB*1.3125 / numberOfPixels);
		break;
	case TUNGSTEN:
		Raver = (sumR / numberOfPixels);
		Gaver = (sumG*1.0078125 / numberOfPixels);
		Baver = (sumB*1.28125 / numberOfPixels);
		break;
	default:
		break;
	}
	float  K = (Raver + Gaver + Baver) / 3;
	float Kr = K / Raver;
	float Kg = K / Gaver;
	float Kb = K / Baver;
	for (int y = 0; y < height; y++)
	{
		uchar *data = f_image.ptr<uchar>(y);

		for (int x = 0; x < width; x++)
		{
			data[x * 3] = min(255,data[x * 3]*Kb);
			data[x * 3 + 1] =min(255, data[x * 3 + 1]*Kg);
			data[x * 3 + 2] =min(255, data[x * 3 + 2]*Kr);
			
		}
	}
	return 0;
}
int  GrayWorld::perfectReflector(cv::Mat& f_image)
{
	int HistRGB[768] = { 0 };
	int height = f_image.size().height;
	int width = f_image.size().width;
	short* SumP = new short[height*width];
	memset(SumP, 0, height*width*sizeof(short));
	short* pSumP = SumP;
	int nmaxsum = 0;
	for (int Y = 0; Y < height; Y++)
	{
		uchar *data = f_image.ptr<uchar>(Y);
		for (int X = 0; X < width; X++)
		{
			int bn = data[X * 3];
			int gn = data[X * 3 + 1];
			int rn = data[X * 3 + 2];
			int Sum = bn + gn + rn;
			if (Sum > nmaxsum)
				nmaxsum = Sum;

			HistRGB[Sum]++;
			*pSumP = (short)Sum;
			pSumP++;
		}
	}
	float Ratio = 10;
	int Threshold = 0;
	int Sum = 0;
	for (int Y = 767; Y >= 0; Y--)
	{
		 Sum += HistRGB[Y];
		if (Sum > width * height * Ratio / 100)
		{
			Threshold = Y;
			break;
		}
	}
	int AvgB = 0;
	int AvgG = 0;
	int AvgR = 0;
	int Amount = 0;
	pSumP = SumP;
	for (int Y = 0; Y < height; Y++)
	{
		uchar *data = f_image.ptr<uchar>(Y);
		for (int X = 0; X < width; X++)
		{
			if (*pSumP >= Threshold)
			{
				AvgB += data[X * 3];
				AvgG += data[X * 3+1];
				AvgR += data[X * 3+2];             // 为获得增益做准备
				Amount++;
			}			
			pSumP++;
		}
	}
	AvgB /= Amount;
	AvgG /= Amount;
	AvgR /= Amount;
	int MaxValue = 255;
	for (int Y = 0; Y < height; Y++)
	{
		uchar *data = f_image.ptr<uchar>(Y);
		for (int X = 0; X < width; X++)
		{
			int Blue = data[X * 3] * MaxValue / AvgB;                                   // 另外一种算法需要先计算不抑制重新计算的RGB的范围，然后求RGB的最大值，如果最大值大于255，则所有的结果都要除以最大值在乘以255，但实际表明该算法、   不合适；
			int Green = data[X * 3+1] * MaxValue / AvgG;
			int Red = data[X * 3+2] * MaxValue / AvgR;
			if (Red > 255) Red = 255; else if (Red < 0) Red = 0;                // 这里需要判断，因为RGB空间所有的颜色转换到YCbCr后，并不是填充满了0-255的范围的，反转过去就会存在一些溢出的点。
			if (Green > 255) Green = 255; else if (Green < 0) Green = 0;        // 编译后应该比三目运算符的效率高
			if (Blue > 255) Blue = 255; else if (Blue < 0) Blue = 0;
			data[X * 3] = (byte)Blue;
			data[X * 3+1] = (byte)Green;
			data[X * 3+2] = (byte)Red;
			
		}
	}
	delete[] SumP;
	return 0;
}
/**
* @brief get the min/max of an unsigned char array
*
* @param data input array
* @param size array size
* @param ptr_min, ptr_max pointers to the returned values, ignored if NULL
*/
static void minmax_u8(const unsigned char *data, int width,int height,int widthstep,
	unsigned char *ptr_min, unsigned char *ptr_max)
{
	unsigned char min, max;
	size_t i;

	/* compute min and max */
	min = data[0];
	max = data[0];
	for (i = 1; i < height; i++) {
		unsigned char *pdata = (unsigned char *)data + widthstep*i;
		for (int j = 0; j < width;j++)
		{
			if (data[j] < min)
				min = data[i];
			if (data[j] > max)
				max = data[i];
		}
		
	}

	/* save min and max to the returned pointers if available */
	if (NULL != ptr_min)
		*ptr_min = min;
	if (NULL != ptr_max)
		*ptr_max = max;
	return;
}
/**
* @brief get quantiles from an unsigned char array such that a given
* number of pixels is out of this interval
*
* This function computes min (resp. max) such that the number of
* pixels < min (resp. > max) is inferior or equal to nb_min
* (resp. nb_max). It uses an histogram algorithm.
*
* @param data input/output
* @param size data array size
* @param nb_min, nb_max number of pixels to flatten
* @param ptr_min, ptr_max computed min/max output, ignored if NULL
*/
static void quantiles_u8(cv::Mat& f_image,
	size_t nb_min, size_t nb_max,
	unsigned char *ptr_min, unsigned char *ptr_max)
{
	/*
	* the histogram must hold all possible "unsigned char" values,
	* including 0
	*/
	size_t h_size = UCHAR_MAX + 1;
	size_t histo[UCHAR_MAX + 1];
	size_t i;

	/* make a cumulative histogram */
	memset(histo, 0x00, h_size * sizeof(size_t));
	int width = f_image.cols;
	int height = f_image.rows;
	size_t size = width*height;
	for (int y = 0; y < height;y++)
	{
		uchar *data = f_image.ptr<uchar>(y);
		for (int x = 0; x < width;x++)
		{
			histo[(size_t)data[x]] += 1;
		}
	}
	for (i = 1; i < h_size; i++)
		histo[i] += histo[i - 1];

	/* get the new min/max */

	if (NULL != ptr_min) {
		/* simple forward traversal of the cumulative histogram */
		/* search the first value > nb_min */
		i = 0;
		while (i < h_size && histo[i] <= nb_min)
			i++;
		/* the corresponding histogram value is the current cell position */
		*ptr_min = (unsigned char)i;
	}

	if (NULL != ptr_max) {
		/* simple backward traversal of the cumulative histogram */
		/* search the first value <= size - nb_max */
		i = h_size - 1;
		/* i is unsigned, we check i<h_size instead of i>=0 */
		while (i < h_size && histo[i] >(size - nb_max))
			i--;
		/*
		* if we are not at the end of the histogram,
		* get to the next cell,
		* the last (backward) value > size - nb_max
		*/
		if (i < h_size - 1)
			i++;
		*ptr_max = (unsigned char)i;
	}
	return;
}
static unsigned char *rescale_u8(cv::Mat& f_image, 
	unsigned char min, unsigned char max)
{
	size_t i;
	int width = f_image.cols;
	int height = f_image.rows;

	if (max <= min)
	{
		for (int y = 0; y < height;y++)
		{
			uchar *data = f_image.ptr<uchar>(y);
			for (int x = 0; x < width; x++)
			{
				data[i] = UCHAR_MAX / 2;
			}
		}
	}		
	else {
		/* build a normalization table */
		unsigned char norm[UCHAR_MAX + 1];
		for (i = 0; i < min; i++)
			norm[i] = 0;
		for (i = min; i < max; i++)
			/*
			* we can't store and reuse UCHAR_MAX / (max - min) because
			*     105 * 255 / 126.            -> 212.5, rounded to 213
			*     105 * (double) (255 / 126.) -> 212.4999, rounded to 212
			*/
			norm[i] = (unsigned char)((i - min) * UCHAR_MAX
			/ (double)(max - min) + .5);
		for (i = max; i < UCHAR_MAX + 1; i++)
			norm[i] = UCHAR_MAX;
		/* use the normalization table to transform the data */
		
			for (int y = 0; y < height; y++)
			{
				uchar *data = f_image.ptr<uchar>(y);
				for (int x = 0; x < width; x++)
				{
					data[x] = norm[(size_t)data[x]];;  
				}
			}
			
	}

}
void balance_u8(cv::Mat& f_image, 
	size_t nb_min, size_t nb_max)
{
	int size = f_image.rows*f_image.cols;
	if (nb_min + nb_max >= size) {
		nb_min = (size - 1) / 2;
		nb_max = (size - 1) / 2;	
	}
	unsigned char min, max;
	/* get the min/max */
	if (0 != nb_min || 0 != nb_max)
		quantiles_u8(f_image, nb_min, nb_max, &min, &max);
	else
		minmax_u8(f_image.data, f_image.cols, f_image.rows, f_image.cols, &min, &max);
	rescale_u8(f_image, min, max);
}
int  GrayWorld::colorbalance_rgb_u8(cv::Mat& f_image)
{
	std::vector<cv::Mat> channels;
	cv::split(f_image, channels);
	int width = f_image.cols;
	int height = f_image.rows;
	int size = width*height;
	int nb_min = size * (10 / 100.);
	int nb_max = size * (20 / 100.);
	balance_u8(channels[0], nb_min, nb_max);
	balance_u8(channels[1], nb_min, nb_max);
	balance_u8(channels[2], nb_min, nb_max);
	cv::merge(channels, f_image);
	return 0;
}