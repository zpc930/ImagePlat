#pragma once
#include <opencv2\opencv.hpp>
class SSR
{
public:
	SSR();
	~SSR();
	void vInitiate();
	int vSSR(cv::Mat& f_image);
	double dCalConvolution(float *ImgData, int ImgWidth, int ImgHeight, int x, int y);
private:
	 int COREWIDTH;
	 int COREHEIGHT;
	 int  COREPIXS;
	 double *mF_Gauss;  //���ȼ���ø�˹��
	 double mC; //����C	
	 double mB; //�����Աȶ�
};

