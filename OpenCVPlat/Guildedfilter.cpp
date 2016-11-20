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
int Guildedfilter::GuidedFilterMat(const  cv::Mat I, cv::Mat p, const int r, const float eps)
{
	int H = I.cols;
	int W = I.rows;
	int nDepth = MatDepthtoTMatrixDpth(I.depth());
	TMatrix* pTI = CreateMatrix(W, H,nDepth, I.channels(), false);
	ConvertMat2Matrix(I, pTI);
	TMatrix* pTP = CreateMatrix(W, H, nDepth, I.channels(), false);
	ConvertMat2Matrix(I, pTP);
	GuidedFilterProcess(pTI, pTP, r, eps);
	return 0;
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
		TMatrix* mean_I = BoxFilter(I, r);
		TMatrix* pMeanId = DividMatrix64F(mean_I, N);
		FreeMatrix(mean_I);
		mean_I = pMeanId;
		TMatrix* mean_p = BoxFilter(p, r);
		TMatrix* pMeanPd = DividMatrix64F(mean_p, N);		
		mean_p = pMeanPd;
		FreeMatrix(mean_p);
		TMatrix* tmp = MultiplyMatrix64F((TMatrix *)I, (TMatrix *)p);
		TMatrix* mean_Ip = BoxFilter(tmp, r);

	}
	return NULL;
}