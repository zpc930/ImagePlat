#pragma once
enum EdgeMode
{
	Tile = 0,				//	重复边缘像素
	Smear = 1				//	镜像数据
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
	int Width;				//	矩阵的宽度
	int Height;				//	矩阵的高度
	int Channel;			//	矩阵通道数
	int Depth;				//	矩阵元素的类型
	int WidthStep;			//	矩阵一行元素的占用的字节数
	int RowAligned;			//	是否使用四字节对齐
	unsigned char *Data;	//	矩阵的数据
};

