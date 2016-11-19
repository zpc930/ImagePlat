#include "stdafx.h"
#include "BoxFilter2.h"


BoxFilter2::BoxFilter2()
{
	f_sum = 0;
	f_sum2 = 0;
	buff = NULL;
	buff2 = NULL;
}


BoxFilter2::~BoxFilter2()
{
	if (f_sum)    delete[] f_sum;
	if (f_sum2)    delete[] f_sum2;
	if (buff)    delete[] buff;
	if (buff2)    delete[] buff2;
}

void BoxFilter2::init(int width, int height, int mwidth, int mheight)
{
	mwidth = mwidth;
	mheight = mheight;
	width = width;
	height = height;

	boxwidth = width - mwidth;
	boxheight = height - mheight;
	f_sum = new int[boxwidth* boxheight];
	f_sum2 = new int[boxwidth*boxheight];
	buff = new int[width];
	buff2 = new int[width];
}

int   BoxFilter2::getLocalSize()
{
	return mwidth > mheight ? mwidth : mheight;
}

int BoxFilter2::getSum(int x, int y)
{
	if (y > mheight / 2 && y<height - mheight / 2 && x>mwidth / 2 && x < width - mwidth / 2)
		return f_sum[(y - mheight / 2) *boxwidth + (x - mwidth / 2)];
	else
		return -1;
	
}
int BoxFilter2::getSquareSum(int x, int y)
{
	if (y > mheight / 2 && y<height - mheight / 2 && x>mwidth / 2 && x < width - mwidth / 2)
		return f_sum2[(y - mheight / 2) *boxwidth + (x - mwidth / 2)];
	else
		return -1;
}
float BoxFilter2::getMean(int x, int y)
{
	return getSum(x, y) / (float)(mwidth*mheight);
}

float BoxFilter2::getVar(int x, int y)
{
	float mean = getMean(x, y);
	return (float)getSquareSum(x, y) / (mwidth *mheight) - mean*mean;
}

void BoxFilter2::boxfilter(unsigned char* img)
{
	int j, x, y;
	memset(buff, 0, width *sizeof(int));
	memset(buff2, 0, width *sizeof(int));
	memset(f_sum, 0, boxwidth *boxheight);
	memset(f_sum2, 0, boxwidth *boxheight);
	for ( y = 0; y < mheight;y++)//第一个box高度行的的和和平方和
	{
		for (x = 0; x < width;x++)
		{
			unsigned char pixel = img[y*width + x];
			buff[x] += pixel;
			buff2[x] += pixel*pixel;
		}
	}
	for (y = 0; y < height - mheight;y++)
	{
		int Xsum = 0;
		int Xsum2 = 0;
		for (j = 0; j < mwidth;j++)
		{
			Xsum += buff[j];
			Xsum2 += buff2[j];
		}
		for (x = 0; x < width - mwidth; x++)
		{
			if (x != 0)
			{
				Xsum = Xsum - buff[x - 1] + buff[mwidth - 1 + x];
				Xsum2 = Xsum2 - buff2[x - 1] + buff2[mwidth - 1 + x];
			}
			f_sum[y*(width - mwidth) + x] = Xsum;
			f_sum2[y*(width - mwidth) + x] = Xsum2;
		}
		for (x = 0; x < width; x++)
		{
			unsigned char pixel = img[y *width + x];
			unsigned char pixel2 = img[(y + mheight) *width + x];
			buff[x] = buff[x] - pixel + pixel2;
			buff2[x] = buff2[x] - pixel*pixel + pixel2*pixel2;
		}
	}
}