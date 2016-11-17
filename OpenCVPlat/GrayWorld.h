#pragma once
#include <opencv2\opencv.hpp>
enum WB_PRESET{
	//�Զ���ƽ��
	AUTO,
	//���� 7500k
	CLOUDY,
	//�չ� 6500k
	DAYLIGHT,
	//���ȹ� 5000k
	INCANDESCENCE,
	//�չ�� 4400k
	FLUORESCENT,
	//��˿�� 2800k
	TUNGSTEN,
};
class GrayWorld
{
public:
	GrayWorld();
	~GrayWorld();
	static int Process(cv::Mat& f_image);
	static int perfectReflector(cv::Mat& f_image);
	static int colorbalance_rgb_u8(cv::Mat& f_image);
};

