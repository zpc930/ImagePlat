#include "stdafx.h"
#include "Guildedfilter.h"
#include "Core.h"
#include "MatToTMatrix.h"
Guildedfilter::Guildedfilter()
{
}


Guildedfilter::~Guildedfilter()
{
}
TMatrix*  Guildedfilter::CumSum(const TMatrix* src, const int d)
{
	int H = src->Height;
	int W = src->Width;
	TMatrix* dest = CreateMatrix(W, H, src->Depth, src->Channel, false);
	memset(dest->Data, 0, dest->Height*dest->WidthStep);
	if (d == 1) {
		// summation over column
		for (int y = 0; y < H; y++) {
			double* curData = (double*)(dest->Data+y*dest->WidthStep);
			double* preData = (double*)(dest->Data + y*dest->WidthStep);
			if (y) {
				// not first row
				preData = (double*)(dest->Data + (y-1)*dest->WidthStep);
			}
			double* srcData = (double*)(src->Data + y*src->WidthStep);
			for (int x = 0; x < W; x++) {
				curData[x] = preData[x] + srcData[x];
			}
		}
	}
	else {
		// summation over row
		for (int y = 0; y < H; y++) {
			double* curData = (double*)(dest->Data + y*dest->WidthStep);
			double* srcData = (double*)(src->Data + y*src->WidthStep);
			for (int x = 0; x < W; x++) {
				if (x) {
					curData[x] = curData[x - 1] + srcData[x];
				}
				else {
					curData[x] = srcData[x];
				}
			}
		}
	}
	return dest;
}


//  %   BOXFILTER   O(1) time box filtering using cumulative sum
//	%
//	%   - Definition imDst(x, y)=sum(sum(imSrc(x-r:x+r,y-r:y+r)));
//  %   - Running time independent of r; 
//  %   - Equivalent to the function: colfilt(imSrc, [2*r+1, 2*r+1], 'sliding', @sum);
//  %   - But much faster.
TMatrix*  Guildedfilter::BoxFilter(const TMatrix* imSrc, const int r)
{
	int H = imSrc->Height;
	int W = imSrc->Width;
	TMatrix* imDst = CreateMatrix(W, H, imSrc->Depth, imSrc->Channel, false);
	memset(imDst->Data, 0, imDst->Height*imDst->WidthStep);
	// cumulative sum over Y axis
	TMatrix* imCum = CumSum(imSrc, 1);
	// difference along Y ( [ 0, r ], [r + 1, H - r - 1], [ H - r, H ] )
	for (int y = 0; y < r + 1; y++) {
		double* dstData = (double*)(imDst->Data + y*imDst->WidthStep);
		double* plusData = (double*)(imCum->Data + (y+r)*imCum->WidthStep); 
		for (int x = 0; x < W; x++) {
			dstData[x] = plusData[x];
		}
	}
	for (int y = r + 1; y < H - r; y++) {
		double* dstData = (double*)(imDst->Data + y*imDst->WidthStep);
		double* minusData = (double*)(imCum->Data + (y - r - 1)*imCum->WidthStep); 
		double* plusData = (double*)(imCum->Data + (y + r)*imCum->WidthStep);
		for (int x = 0; x < W; x++) {
			dstData[x] = plusData[x] - minusData[x];
		}
	}
	for (int y = H - r; y < H; y++) {
		double* dstData = (double*)(imDst->Data + y*imDst->WidthStep);
		double* minusData = (double*)(imCum->Data + (y - r - 1)*imCum->WidthStep);
		double* plusData = (double*)(imCum->Data + (H-1)*imCum->WidthStep);
		for (int x = 0; x < W; x++) {
			dstData[x] = plusData[x] - minusData[x];
		}
	}
	// cumulative sum over X axis
	FreeMatrix(imCum);
	imCum = CumSum(imDst, 2);
	for (int y = 0; y < H; y++) {
		double* dstData = (double*)(imDst->Data + y*imDst->WidthStep);
		double* cumData = (double*)(imCum->Data + (y)*imCum->WidthStep);
		for (int x = 0; x < r + 1; x++) {
			dstData[x] = cumData[x + r];
		}
		for (int x = r + 1; x < W - r; x++) {
			dstData[x] = cumData[x + r] - cumData[x - r - 1];
		}
		for (int x = W - r; x < W; x++) {
			dstData[x] = cumData[W - 1] - cumData[x - r - 1];
		}
	}
	return imDst;
}
#include <opencv2/highgui.hpp>
void ShowMtarix(TMatrix* I,char* sztitle)
{
	int H = I->Height;
	int W = I->Width;
	cv::Mat M;
	M.create(W, H, CV_64FC1);
	ConvertMatrix2Mat(I, M);
	cv::imshow(sztitle, M);
}
cv::Mat  Guildedfilter::GuidedFilterMat(const  cv::Mat I, cv::Mat p, const int r, const float eps)
{
	int H = I.cols;
	int W = I.rows;
	int nDepth = MatDepthtoTMatrixDpth(I.depth());
	TMatrix* pTI = CreateMatrix(W, H,nDepth, I.channels(), false);
	ConvertMat2Matrix(I, pTI);
	TMatrix* pTP = CreateMatrix(W, H, nDepth, I.channels(), false);
	ConvertMat2Matrix(I, pTP);	
	TMatrix* pRseult =GuidedFilterProcess(pTI, pTP, r, eps);
	FreeMatrix(pTI);
	FreeMatrix(pTP);
	cv::Mat dst = I.clone();
	ConvertMatrix2Mat(pRseult,  dst);
	return dst;
}


TMatrix* Guildedfilter::GuidedFilterProcess(const  TMatrix* I, const TMatrix* p, const int r, const float eps)
{
	int H = I->Height;
	int W = I->Width;
	TMatrix* N = CreateMatrix(W, H, I->Depth, I->Channel, false);
	MatrixFill(N, 1.0);
	TMatrix* M = BoxFilter(N, r);
	FreeMatrix(N);
	N = M;
	if (1 == I->Channel)
	{
		TMatrix* pMeanId  = BoxFilter(I, r);
		TMatrix* mean_I = DividMatrix64F(pMeanId, N);
		FreeMatrix(pMeanId);
		
		TMatrix* pMeanPd  = BoxFilter(p, r);
		TMatrix*  mean_p = DividMatrix64F(pMeanPd, N);		
		FreeMatrix(pMeanPd);
		

		TMatrix* tmp = MultiplyMatrix64F((TMatrix *)I, (TMatrix *)p);
		  pMeanPd = BoxFilter(tmp, r);
		FreeMatrix(tmp);
		TMatrix* mean_Ip = DividMatrix64F(pMeanPd, N);
		FreeMatrix(pMeanPd);

		
		tmp=MultiplyMatrix64F(mean_I, mean_p);
		TMatrix* cov_Ip = SubMatrix64F(mean_Ip , tmp);
		FreeMatrix(tmp);
		FreeMatrix(mean_Ip);

		tmp = MultiplyMatrix64F((TMatrix *)I, (TMatrix *)I);
		TMatrix* mean_II = BoxFilter(tmp, r);
		FreeMatrix(tmp);
		tmp = mean_II;
		mean_II = DividMatrix64F(tmp, N);
		FreeMatrix(tmp);

		tmp=MultiplyMatrix64F(mean_I, mean_I);
		TMatrix* var_I = SubMatrix64F(mean_II , tmp);
		FreeMatrix(mean_II);

		TMatrix* epsMat = CreateMatrix(W, H, I->Depth, I->Channel, false);
		MatrixFill(epsMat, eps);
		TMatrix* var_temp = AddMatrix64F(var_I, epsMat);
		TMatrix* a = DividMatrix64F(cov_Ip, var_temp);

		FreeMatrix(var_temp);
		FreeMatrix(tmp);
		FreeMatrix(cov_Ip);
		FreeMatrix(epsMat);
		FreeMatrix(var_I);

		tmp = MultiplyMatrix64F(a, mean_I);
		TMatrix* b = SubMatrix64F(mean_p, tmp);  
		TMatrix* temp_mean_a = BoxFilter(a, r);
		TMatrix* mean_a = DividMatrix64F(temp_mean_a, N);
		FreeMatrix(temp_mean_a);
		FreeMatrix(a);
		FreeMatrix(mean_I);
		FreeMatrix(mean_p);
		
		TMatrix* temp_mean_b = BoxFilter(b, r);		
		TMatrix* mean_b = DividMatrix64F(temp_mean_b, N);
		FreeMatrix(tmp);
		FreeMatrix(temp_mean_b);
		FreeMatrix(b);
		FreeMatrix(N);
		tmp = MultiplyMatrix64F(mean_a, (TMatrix *)I);
		FreeMatrix(mean_a);
		TMatrix* q = AddMatrix64F( tmp , mean_b);
		FreeMatrix(mean_b);
		FreeMatrix(tmp);
		return q;
	}
	return NULL;
}