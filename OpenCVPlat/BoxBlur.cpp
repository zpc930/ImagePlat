#include "stdafx.h"
#include "BoxBlur.h"


BoxBlur::BoxBlur()
{
}


BoxBlur::~BoxBlur()
{
}
/// <summary>
/// �����ڴ溯��,��32�ֽڶ��롣
/// </summary>
/// <param name="Size">��Ҫʹ�õ��ڴ��С�����ֽ�Ϊ��λ����</param>
/// <param name="ZeroMemory">�ڴ������Ƿ�������㴦�� ��</param>
/// <returns>�����������ڴ��ָ�룬ʧ���򷵻�NULL��</returns>
void *AllocMemory(unsigned int Size, bool ZeroMemory)	//	https://technet.microsoft.com/zh-cn/library/8z34s9c6
{
	void *Ptr = _aligned_malloc(Size, 32);					//	����SSE,AVX�ȸ߼����������󣬷�����ʼ��ַʹ��32�ֽڶ��롣��ʵ_mm_malloc�����������
	if (Ptr != NULL && ZeroMemory == true)
		memset(Ptr, 0, Size);
	return Ptr;
}
/// <summary>
/// �ͷ��ڴ档
/// </summary>
/// <param name="Ptr">�ڴ����ݵĵ�ַ��</param>
void FreeMemory(void *Ptr)
{
	if (Ptr != NULL) _aligned_free(Ptr);		//	_mm_free���Ǹú���
}
int *GetExpandPos(int Length, int Left, int Right, EdgeMode Edge)
{
	if (Left < 0 || Length < 0 || Right < 0) return NULL;
	int *Pos = (int *)AllocMemory((Left + Length + Right) * sizeof(int), false);
	if (Pos == NULL) return NULL;

	int X, PosX;
	for (X = -Left; X < Length + Right; X++)
	{
		if (X < 0)
		{
			if (Edge == EdgeMode::Tile)							//�ظ���Ե����
				Pos[X + Left] = 0;
			else
			{
				PosX = -X;
				while (PosX >= Length) PosX -= Length;			// ����������
				Pos[X + Left] = PosX;
			}
		}
		else if (X >= Length)
		{
			if (Edge == EdgeMode::Tile)
				Pos[X + Left] = Length - 1;
			else
			{
				PosX = Length - (X - Length + 2);
				while (PosX < 0) PosX += Length;
				Pos[X + Left] = PosX;
			}
		}
		else
		{
			Pos[X + Left] = X;
		}
	}
	return Pos;
}
int BoxBlur::Process(cv::Mat &image, int Radius, EdgeMode Edge)
{
	int X, Y, Z, Width, Height, Channel, Index;
	int Value, ValueB, ValueG, ValueR;
	int *RowPos, *ColPos, *ColSum, *Diff;
	Width = image.cols, Height = image.rows, Channel = image.channels();
	int Size = 2 * Radius + 1, Amount = Size * Size, HalfAmount = Amount / 2;
	RowPos = GetExpandPos(Width, Radius, Radius, Edge);
	ColPos = GetExpandPos(Height, Radius, Radius, Edge);
	ColSum = (int *)AllocMemory(Width * Channel * sizeof(int), true);
	Diff = (int *)AllocMemory((Width - 1) * Channel * sizeof(int), true);
	unsigned char *RowData = (unsigned char *)AllocMemory((Width + 2 * Radius) * Channel, true);
	cv::Mat Sum;
	Sum.create(Height, Width, Channel == 3 ? CV_32SC3 : CV_32SC1);
	int nSize =Sum.total();
	for (Y = 0; Y < Height; Y++)
	{
		uchar *data = image.ptr<uchar>(Y);
		int *datasum = Sum.ptr<int >(Y);
		//	����һ�����ݼ���Ե���ֲ��ֵ���ʱ�Ļ�������
		if (Channel == 1)
		{
			for (X = 0; X < Radius; X++)
				RowData[X] = data[RowPos[X]];
			memcpy(RowData + Radius, data, Width);
			for (X = Radius + Width; X < Radius + Width + Radius; X++)
				RowData[X] = data[RowPos[X]];
		}
		else if (Channel==3)
		{
			for (X = 0; X < Radius; X++)
			{
				Index = RowPos[X] * 3;
				RowData[X * 3] = data[Index];
				RowData[X * 3 + 1] = data[Index + 1];
				RowData[X * 3 + 2] = data[Index + 2];
			}
			memcpy(RowData + Radius * 3, data, Width * 3);
			for (X = Radius + Width; X < Radius + Width + Radius; X++)
			{
				Index = RowPos[X] * 3;
				RowData[X * 3 + 0] = data[Index + 0];
				RowData[X * 3 + 1] = data[Index + 1];
				RowData[X * 3 + 2] = data[Index + 2];
			}
		}
		unsigned char *AddPos = RowData + Size * Channel;
		unsigned char *SubPos = RowData;
		for (X = 0; X < (Width - 1) * Channel; X++)
			Diff[X] = AddPos[X] - SubPos[X];
		//	��һ����Ҫ���⴦��
		if (Channel == 1)
		{
			for (Z = 0, Value = 0; Z < Size; Z++)	Value += RowData[Z];
			datasum[0] = Value;

			for (X = 1; X < Width; X++)
			{
				Value += Diff[X - 1];	datasum[X] = Value;				//	����·�����ٶ�������ߺܶ�
			}
		}
		else if (Channel == 3)
		{
			for (Z = 0, ValueB = ValueG = ValueR = 0; Z < Size; Z++)
			{
				ValueB += RowData[Z * 3 + 0];
				ValueG += RowData[Z * 3 + 1];
				ValueR += RowData[Z * 3 + 2];
			}
			datasum[0] = ValueB;	datasum[1] = ValueG;	datasum[2] = ValueR;

			for (X = 1; X < Width; X++)
			{
				Index = X * 3;
				ValueB += Diff[Index - 3];		
				datasum[Index + 0] = ValueB;
				ValueG += Diff[Index - 2];		
				datasum[Index + 1] = ValueG;
				ValueR += Diff[Index - 1];	
				datasum[Index + 2] = ValueR;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////

	for (Y = 0; Y < Size - 1; Y++)			//	ע��û�����һ��Ŷ						//	����ĺ�ʱֻռ������15%����
	{
		int *LinePS = Sum.ptr<int >(ColPos[Y]);
		for (X = 0; X < Width * Channel; X++)	ColSum[X] += LinePS[X];
	}

	for (Y = 0; Y < Height; Y++)
	{
		unsigned char* LinePD = image.ptr<uchar>(Y);
		int *AddPos = (int*)Sum.ptr<int >(ColPos[Y + Size - 1]);
		int *SubPos = (int*)Sum.ptr<int >(ColPos[Y ]);

		for (X = 0; X < Width * Channel; X++)
		{
			Value = ColSum[X] + AddPos[X];
			LinePD[X] = (Value + HalfAmount) / Amount;					//		+  HalfAmount ��Ҫ��Ϊ����������
			ColSum[X] = Value - SubPos[X];
		}
	}
	FreeMemory(RowPos);
	FreeMemory(ColPos);
	FreeMemory(Diff);	
	FreeMemory(ColSum);
	FreeMemory(RowData);
	Sum.release();
	return 0;
}

// <summary> 
/// ���������������������㹫ʽΪ�� 
///     (p6 - p6*p7*p0) + sigma(pk - pk*p(k+1)*p(k+2)), k = {0,2,4) 
/// </summary> 
/// <param name="list"></param> 
/// <returns></returns> 
int DetectConnectivity(int* list)
{
	 int count = list[6] - list[6] * list[7] * list[0];
	count += list[0] - list[0] * list[1] * list[2];
	count += list[2] - list[2] * list[3] * list[4];
	count += list[4] - list[4] * list[5] * list[6];
	return count;
}
void FillNeighbors(uchar* p, int* list, int width, uchar foreground = 255)
{
	// list �洢���ǲ�������ǰ����Ϊ0��������Ϊ1���Է����������ļ���

	list[0] = p[1] == foreground ? 0 : 1;
	list[1] = p[1 - width] == foreground ? 0 : 1;
	list[2] = p[-width] == foreground ? 0 : 1;
	list[3] = p[-1 - width] == foreground ? 0 : 1;
	list[4] = p[-1] == foreground ? 0 : 1;
	list[5] = p[-1 + width] == foreground ? 0 : 1;
	list[6] = p[width] == foreground ? 0 : 1;
	list[7] = p[1 + width] == foreground ? 0 : 1;
}
int BoxBlur::Hilditch(cv::Mat &image)
{

	return 0;
}