#pragma once
#include "CommonHeader.h"
#include <opencv/cv.h>
class Guildedfilter
{
public:
	Guildedfilter();
	~Guildedfilter();
	TMatrix*  CumSum(const TMatrix* src, const int d);
	TMatrix* BoxFilter(const TMatrix* imSrc, const int r);
	//  %   GUIDEDFILTER   O(1) time implementation of guided filter.
		//	%
		//	%   - guidance image: I (should be a gray-scale/single channel image)
		//	%   - filtering input image: p (should be a gray-scale/single channel image)
		//	%   - local window radius: r
		//	%   - regularization parameter: eps
	TMatrix* GuidedFilterProcess(const  TMatrix* I, const TMatrix* p, const int r, const float eps);
	int GuidedFilterMat(const  cv::Mat I, cv::Mat p, const int r, const float eps);
};

