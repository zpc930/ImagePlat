#pragma once
#include <opencv/cv.h>
#include "Core.h"
int MatDepthtoTMatrixDpth(int nMatDepth);
int ConvertMat2Matrix(cv::Mat src, TMatrix* pDst);
