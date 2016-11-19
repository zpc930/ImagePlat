#pragma once
enum EdgeMode
{
	Tile = 0,				//	�ظ���Ե����
	Smear = 1				//	��������
};
enum DEPTH
{
	DEPTH_8U, 				//	unsigned char
	DEPTH_8S, 				//	char
	DEPTH_16S,				//	short
	DEPTH_32S,				//  int
	DEPTH_32F,				//	float
	DEPTH_64F				//	double
};
struct TMatrix
{
	int Width;					//	����Ŀ��
	int Height;					//	����ĸ߶�
	int WidthStep;				//	����һ��Ԫ�ص�ռ�õ��ֽ���
	int Channel;				//	����ͨ����
	int Depth;					//	����Ԫ�ص�����
	unsigned char *Data;		//	���������
	int Reserved;				//	����ʹ��
};

