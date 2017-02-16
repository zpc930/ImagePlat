#pragma once
#include "CommonHeader.h"
#include "opencv/cv.h"
class cprgb2gray
{
public:
	cprgb2gray();
	~cprgb2gray();
	static int declor(const  cv::Mat I, cv::Mat p,float sigma);
protected:
	void init();
	void grad_system(TMatrix* img, std::vector <  std::vector < double > > &polyGrad,
		std::vector < double > &Cg, std::vector <  std::vector <int> >& comb);
private:
	float msigma;
	int morder;
	TMatrix* mKernalx;
	TMatrix* mKernaly;
};

