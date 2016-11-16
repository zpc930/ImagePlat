#pragma once
enum EdgeMode
{
	Tile = 0,				//	重复边缘像素
	Smear = 1				//	镜像数据
};

struct TMatrix
{
	int Width;					//	矩阵的宽度
	int Height;					//	矩阵的高度
	int WidthStep;				//	矩阵一行元素的占用的字节数
	int Channel;				//	矩阵通道数
	int Depth;					//	矩阵元素的类型
	unsigned char *Data;		//	矩阵的数据
	int Reserved;				//	保留使用
};