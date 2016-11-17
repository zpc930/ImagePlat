#include "stdafx.h"
#include "SSR.h"
#include "ColorConvert.h"

SSR::SSR()
{
	mF_Gauss = NULL;
	mC = 5;
	mB = 170;
	COREWIDTH =mC * 2 + 1;
	COREHEIGHT = mC * 2 + 1;
	COREPIXS = COREWIDTH*COREHEIGHT;
}


SSR::~SSR()
{
}
void SSR::vInitiate()
{

	if (mF_Gauss != NULL)
	{
		delete[] mF_Gauss;
	}
	mF_Gauss = new double[COREPIXS];
	int centrey = COREHEIGHT / 2;
	int centrex = COREWIDTH / 2;
	double sum = 0.0;
	for (int i = 0; i < COREHEIGHT; ++i)
	{
		for (int j = 0; j < COREWIDTH; ++j)
		{
			mF_Gauss[j + i*COREWIDTH] = exp(-((j - centrex) * (j - centrex) + (i - centrey) * (i - centrey)) / (mC*mC));
			mF_Gauss[j + i*COREWIDTH] = mF_Gauss[j + i*COREWIDTH];// * mF_Gauss[j+i*COREWIDTH];
			sum += mF_Gauss[j + i*COREWIDTH];
		}
	}

	for (int i = 0; i < COREPIXS; ++i)
	{
		mF_Gauss[i] /= sum;
	}
	
}
//计算卷积
double  SSR::dCalConvolution(float *ImgData, int ImgWidth, int ImgHeight, int x, int y)
{
	int y0 = y - COREHEIGHT / 2;
	int x0 = x - COREWIDTH / 2;
	double result = 0.0;
	for (int i = 0; i < COREHEIGHT; ++i)
	{
		for (int j = 0; j < COREWIDTH; ++j)
		{
			int tempx = x0 + j;
			int tempy = y0 + i;
			if (tempx < 0 || tempx >= ImgWidth || tempy < 0 || tempy >= ImgHeight)
			{
				int a = tempy < 0 ? 0 : (tempy >= ImgHeight ? ImgHeight - 1 : tempy)*ImgWidth;
				int b = tempx < 0 ? 0 : (tempx >= ImgWidth ? ImgWidth - 1 : tempx);
				int temp = a + b;
				result += ImgData[temp] * mF_Gauss[i*COREWIDTH + j];
			}
			else
			{
				result += ImgData[tempy*ImgWidth + tempx] * mF_Gauss[i*COREWIDTH + j];
			}
		}
	}
	return result;
}
int SSR::vSSR(cv::Mat& f_image)
{
	int width = f_image.size().width;
	int height = f_image.size().height;
	int nwidthStep = f_image.step;
	uchar *data = f_image.data;
	cv::Mat mhsvimg;
	mhsvimg.create(width, height, CV_32FC3);
	int nhsvwidtstep = mhsvimg.step;
	float* hsvdata = (float*)mhsvimg.data;
	long datasize = width* height;
	float *dSrcImgData = (float *)malloc(sizeof(float)*datasize);
	float *dDstImgData = (float *)malloc(sizeof(float)*datasize);
	float *dSrcLData = (float *)malloc(sizeof(float)*datasize);
	float *dDstLData = (float *)malloc(sizeof(float)*datasize);
	for (int y = 0; y < height;y++)
	{
		uchar *pdata = data + y*nwidthStep;
		float* row_ptr = mhsvimg.ptr<float>(y);
		float* pscr_ptr = dSrcImgData + y*width;
		for (int x = 0; x < width;x++)
		{
			int nb = *pdata++;
			int ng = *pdata++;
			int nr = *pdata++;
			float fh = 0, fs = 0, fv = 0;
			ColorConvert::RGBtoHSV((float)nr, (float)ng, (float)nb, &fh, &fs, &fv);
			*row_ptr = fh/255.0;
			row_ptr++;
			*row_ptr = fs/255.0;
			row_ptr++;
			*row_ptr = fv/255.0;
			row_ptr++;
			*pscr_ptr = fv / 255.0;
			pscr_ptr++;
		}
	}
	//得到目标Log图像
	double minvalue = 10000;
	double maxvalue = 0.0;
	for (int i = 0; i <height; ++i)
	{
		for (int j = 0; j< width; ++j)
		{
			double tempConvolution = dCalConvolution(dSrcImgData, width, height, j, i);
			//double tempLogCon = log(tempConvolution);
			//dDstLData[i*width + j] = dSrcLData[i*width + j] - tempLogCon;
			dDstImgData[i* width + j] = dSrcImgData[i*width + j] / tempConvolution; //exp(dDstLData[i*width + j]);
			if (dDstImgData[i* width + j] > 0.1 && dDstImgData[i* width + j] < 10)
			{
				maxvalue = maxvalue > dDstImgData[i* width + j] ? maxvalue : dDstImgData[i* width + j];
				minvalue = minvalue < dDstImgData[i* width + j] ? minvalue : dDstImgData[i* width + j];
			}
			//dstsum+=dDstImgData[i* width+j];
		}
	}
	for (int i = 0; i<datasize; ++i)
	{
		//dSrcImgData[i] = dDstImgData[i]*dSrcImgData[i];
		dDstImgData[i] = (dDstImgData[i])*mB;//rate;//*170;
	}
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			//dDstImgData[i* width+j] = (dDstImgData[i* width+j]-lb)*rate;//(dSrcImgData[i* width+j]-lb)*rate;
			//uDstImgData[i* widthStep + j] = dDstImgData[i* width + j]>255 ? 255 : dDstImgData[i* width + j];
		}
	}
	return 0;
}
