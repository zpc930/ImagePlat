#include "stdafx.h"
#include "cprgb2gray.h"
#include "MatToTMatrix.h"
#include "arithm.h"
#include "opencv2/highgui/highgui.hpp"
#include "GlobalFun.h"
cprgb2gray::cprgb2gray()
{
	 mKernalx=nullptr;
	 mKernaly=nullptr;
}


cprgb2gray::~cprgb2gray()
{
}
int cprgb2gray::declor(const  cv::Mat I, cv::Mat p, float sigma)
{
	int H = I.rows;  
	int W = I.cols;
	int nDepth = MatDepthtoTMatrixDpth(I.depth());
	int nChannel = I.channels();
	TMatrix* pImg = CreateMatrix(W, H, DEPTH_32F, nChannel, 0);
	int nContinew = H*I.step;
	cvtScale_(I.data, I.step, (float*)pImg->Data, pImg->WidthStep, nContinew, 1, 1 / 255.0, 0.0);
	cv::Mat Img;
	Img.create(H, W, CV_32FC3);
	ConvertMatrix2Mat(pImg, Img);
	cprgb2gray* obj = new cprgb2gray();
	obj->init();
	std::vector <double> Cg;
	std::vector < std::vector <double> > polyGrad;
	std::vector < std::vector < int > > comb;

	//cv::namedWindow("qq3", 0);
	//cv::imshow("qq3", Img);
	return 0;
}

void cprgb2gray::init()
{
	mKernalx = CreateMatrix(2, 1, DEPTH_32F, 1, 0);
	mKernaly = CreateMatrix(1, 2, DEPTH_32F, 1, 0);
	float* pdata = (float*)mKernalx->Data;
	pdata[0] = 1.0f;
	pdata[1] = -1.0f;
	 pdata = (float*)mKernaly->Data;
	pdata[0] = -1.0f;
	pdata[1] = 1.0f;
	
	morder = 2;
	msigma = 0.02f;
}
void cprgb2gray::grad_system(TMatrix* img, std::vector <  std::vector < double > > &polyGrad,
	std::vector < double > &Cg, std::vector <  std::vector <int> >& comb)
{
	int h = img->Height;
	int w = img->Width;
	double sizefactor;
	if ((h + w) > 800)
	{
		sizefactor = (double)800 / (h + w);
	//	resize(img, img, round_num(h*sizefactor), round_num(w*sizefactor));
	}
}