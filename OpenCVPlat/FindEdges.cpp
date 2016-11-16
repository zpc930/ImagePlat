#include "stdafx.h"
#include "FindEdges.h"


FindEdges::FindEdges()
{
}


FindEdges::~FindEdges()
{
}
int FindEdges::mdFindEdges(cv::Mat &image)
{
	int X, Y;
	int Width, Height, Stride, StrideC, HeightC;
	int Speed, SpeedOne, SpeedTwo, SpeedThree;
	int BlueOne, BlueTwo, GreenOne, GreenTwo, RedOne, RedTwo;
	int PowerRed, PowerGreen, PowerBlue;
	Width = image.cols;
	Height = image.rows;
	byte* SqrValue = new byte[65026];
	for (Y = 0; Y < 65026; Y++) SqrValue[Y] = (byte)(255 - (int)sqrt(Y));		
	cv::Mat ImageDataC;
	ImageDataC.create(Height+2, Width+2, CV_8UC3);
	for (Y = 0; Y < Height; Y++)
	{
		uchar *data = image.ptr<uchar>(Y);
		uchar *newdata = ImageDataC.ptr<uchar>(Y+1);
		memcpy(newdata, data,3);
		memcpy(newdata+3, data, Width*3);
		memcpy(newdata + 3+Width*3, data+Width*3, 3);
	}
	{
		uchar *data = ImageDataC.ptr<uchar>(0);
		uchar *llinedata = ImageDataC.ptr<uchar>(1);
		memcpy(llinedata, data, Width + 2);
		data = ImageDataC.ptr<uchar>(Height );
		llinedata = ImageDataC.ptr<uchar>(Height + 1);
		memcpy(llinedata, data, Width + 2);
	}
	
	for (Y = 0; Y < Height; Y++)
	{
		uchar *dataone = ImageDataC.ptr<uchar>(Y);
		uchar *datatwo = ImageDataC.ptr<uchar>(Y+1);
		uchar *datathree = ImageDataC.ptr<uchar>(Y + 2);
		uchar *dataorg = image.ptr<uchar>(Y);
		for (int X = 0; X < Width; X++)
		{
			BlueOne = dataone[X * 3] + 2 * datatwo[X * 3] + datathree[X * 3] - dataone[X * 3 + 6] - 2 * datatwo[X * 3 + 6] - datathree[X * 3 + 6];
			GreenOne = dataone[X * 3 + 1] + 2 * datatwo[X * 3 + 1] + datathree[X * 3 + 1] - dataone[X * 3 + 7] - 2 * datatwo[X * 3 + 7] - datathree[X * 3 + 7];
			RedOne = dataone[X * 3 + 2] + 2 * datatwo[X * 3 + 2] + datathree[X * 3 + 2] - dataone[X * 3 + 8] - 2 * datatwo[X * 3 + 8] - datathree[X * 3 + 8];
			BlueTwo = dataone[X * 3] + 2 * dataone[X * 3 + 3] + dataone[X * 3 + 6] - datathree[X * 3] - 2 * datathree[X * 3 + 3] - datathree[X * 3 + 6];
			GreenTwo = dataone[X * 3 + 1] + 2 * dataone[X * 3 + 4] + dataone[X * 3 + 7] - datathree[X * 3 + 1] - 2 * datathree[X * 3 + 4] - datathree[X * 3 + 7];
			RedTwo = dataone[X * 3 + 2] + 2 * dataone[X * 3 + 5] + dataone[X * 3 + 8] - datathree[X * 3 + 2] - 2 * datathree[X * 3 + 5] - datathree[X * 3 + 8];
			PowerBlue = BlueOne * BlueOne + BlueTwo * BlueTwo;
			PowerGreen = GreenOne * GreenOne + GreenTwo * GreenTwo;
			PowerRed = RedOne * RedOne + RedTwo * RedTwo;

			if (PowerBlue > 65025) PowerBlue = 65025;           //  处理掉溢出值
			if (PowerGreen > 65025) PowerGreen = 65025;
			if (PowerRed > 65025) PowerRed = 65025;
			dataorg[X * 3] = SqrValue[PowerBlue];
			dataorg[X * 3 + 1] = SqrValue[PowerGreen];
			dataorg[X * 3 + 2] = SqrValue[PowerRed];
		}
	}
	delete[] SqrValue;
	ImageDataC.release();
	return 0;
}
int FindEdges::SalientRegionDetectionBasedonLC(cv::Mat &Src)
{
	int Width = Src.cols;
	int Height = Src.rows;
	int X, Y, Index, CurIndex, Value;
	unsigned char *Gray = (unsigned char*)malloc(Width * Height);
	int *Dist = (int *)malloc(256 * sizeof(int));
	int *HistGram = (int *)malloc(256 * sizeof(int));
	float *DistMap = (float *)malloc(Height * Width * sizeof(float));

	memset(HistGram, 0, 256 * sizeof(int));

	for (Y = 0; Y < Height; Y++)
	{
		Index = Y * Width;
		CurIndex = Y * Width;
		uchar *dataone = Src.ptr<uchar>(Y);
		for (X = 0; X < Width; X++)
		{
			Value = (dataone[X * 3] + dataone[X * 3 + 1] * 2 + dataone[X * 3 + 2]) / 4;        //    保留灰度值，以便不需要重复计算
			HistGram[Value] ++;
			Gray[CurIndex] = Value;
			Index += 3;
			CurIndex++;
		}
	}
	for (Y = 0; Y < 256; Y++)
	{
		Value = 0;
		for (X = 0; X < 256; X++)
			Value += abs(Y - X) * HistGram[X];                //    论文公式（9），灰度的距离只有绝对值，这里其实可以优化速度，但计算量不大，没必要了
		Dist[Y] = Value;
	}
	for (Y = 0; Y < Height; Y++)
	{
		CurIndex = Y * Width;		
		for (X = 0; X < Width; X++)
		{
			DistMap[CurIndex] = Dist[Gray[CurIndex]];        //    计算全图每个像素的显著性			
			CurIndex++;
		}
	}
	float fMax = 0;
	float fMin = 999999999;
	for(int i = 0; i < Height*Width; i++)
	{
		if (DistMap[i]>fMax)
		{
			fMax = DistMap[i];
		}
		if (DistMap[i]<fMin)
		{
			fMin = DistMap[i];
		}
	}
	for (int i = 0; i < Height*Width; i++)
	{
		DistMap[i] = (DistMap[i] - fMin) / (fMax-fMin);
	}
	for (Y = 0; Y < Height; Y++)
	{
		uchar *dataone = Src.ptr<uchar>(Y);
		CurIndex = Y * Width;
		for (X = 0; X < Width; X++)
		{
			dataone[X*3] = DistMap[CurIndex]*255;        
			dataone[X * 3+1] = DistMap[CurIndex]*255;
			dataone[X * 3+2] = DistMap[CurIndex]*255;
			CurIndex++;
		}
	}
	return 0;
}