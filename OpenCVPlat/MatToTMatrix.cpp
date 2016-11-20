#include "stdafx.h"
#include "MatToTMatrix.h"

int MatDepthtoTMatrixDpth(int nMatDepth)
{
	int nResult = -1;
	switch (nMatDepth)
	{
	case CV_8U:
		nResult = DEPTH_8U;
		break;
	case CV_8S:
		nResult = DEPTH_8S;
		break;
	case CV_16U:
		nResult = DEPTH_16U;
		break;
	case  CV_16S:
		nResult = DEPTH_16S;
		break;
	case  CV_32S:
		nResult = DEPTH_32S;
	case CV_32F:
		nResult = DEPTH_32F;
		break;
	case  CV_64F:
		nResult = DEPTH_64F;
		break;
	default:
		break;
	}
	return nResult;
}
int CopyMat2Matrix(cv::Mat src, TMatrix* pDst)
{
	int nChannel = src.channels();
	int width = src.rows;
	int height = src.cols;
	int ndestwstep = pDst->WidthStep;
	for (int y = 0; y < height; y++)
	{
		uchar *data = src.ptr<uchar>(y);
		uchar *pdessdata = pDst->Data + ndestwstep*y;
		int nlen = GetElementSize(pDst->Depth)*width*nChannel;
		memcpy(pdessdata, data, nlen);
	}
	return 0;
}
int ConvertMat2Matrix(cv::Mat src, TMatrix* pDst)
{
	int matType =src.depth();
	int nTmatrixType = pDst->Depth;
	if (matType == CV_8U && nTmatrixType == DEPTH_8U)
	{
		CopyMat2Matrix(src, pDst);
	}
	else if (matType == CV_8S && nTmatrixType == DEPTH_8S)
	{
		CopyMat2Matrix(src, pDst);
	}
	else if (matType == CV_16U && nTmatrixType == DEPTH_16U)
	{
		CopyMat2Matrix(src, pDst);
	}
	else if (matType == CV_16S && nTmatrixType == DEPTH_16S)
	{
		CopyMat2Matrix(src, pDst);
	}
	else if (matType == CV_32S && nTmatrixType == DEPTH_32S)
	{
		CopyMat2Matrix(src, pDst);
	}
	else if (matType == CV_32F && nTmatrixType == DEPTH_32F)
	{
		CopyMat2Matrix(src, pDst);
	}
	else if (matType == CV_64F && nTmatrixType == DEPTH_64F)
	{
		CopyMat2Matrix(src, pDst);
	}
	return 0;
}