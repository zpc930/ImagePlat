#pragma once
enum EdgeMode
{
	Tile = 0,				//	�ظ���Ե����
	Smear = 1				//	��������
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