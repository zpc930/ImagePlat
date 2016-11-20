#include "stdAfx.h"
#include "Core.h"
#include "arithm.h"

/// <summary>
/// ���ݾ���Ԫ�ص���������ȡһ��Ԫ��ʵ��ռ�õ��ֽ���
/// </summary>
/// <param name="Depth">����Ԫ�ص����͡�</param>
/// <remarks> ֻ��Ϊ�ڲ�ʹ�á�</remarks>
int GetElementSize(int Depth)
{
	int Size;
	switch (Depth)
	{
	case DEPTH_8U:
		Size = sizeof(unsigned char);
		break;
	case DEPTH_8S:
		Size = sizeof(char);
		break;
	case DEPTH_16U:
		Size = sizeof(unsigned short);
		break;
	case DEPTH_16S:
		Size = sizeof(short);
		break;
	case DEPTH_32S:
		Size = sizeof(int);
		break;
	case DEPTH_32F:
		Size = sizeof(float);
		break;
	case DEPTH_64F:
		Size = sizeof(double);
		break;
	default:
		Size = 0;
		break;
	}
	return Size;
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

/// <summary>
/// �����µľ������ݡ�
/// </summary>
/// <param name="Width">����Ŀ�ȡ�</param>
/// <param name="Height">����ĸ߶ȡ�</param>
/// <param name="Depth">�����Ԫ�����͡�</param>
/// <param name="Channel">�����ͨ������</param>
/// <param name="RowAligned">ÿ���������Ƿ���Ҫ���ֽڶ��롣</param>
/// <returns>���ش����ľ������ݣ�ʧ���򷵻�NULL��</returns>
TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned)
{
	if (Width < 1 || Height < 1) return NULL;
	if (Depth < DEPTH_8U && Depth > DEPTH_64F) return NULL;

	TMatrix * Matrix = (TMatrix *)AllocMemory(sizeof(TMatrix), false);		//	�������������᲻�ɹ�
	Matrix->Width = Width;
	Matrix->Height = Height;
	Matrix->Depth = Depth;
	Matrix->Channel = Channel;
	Matrix->RowAligned = RowAligned;
	if (RowAligned)
		Matrix->WidthStep = WidthBytes(Width * Channel * GetElementSize(Depth));
	else
		Matrix->WidthStep = Width * Channel * GetElementSize(Depth);
	Matrix->Data = (unsigned char *)AllocMemory(Matrix->Height * Matrix->WidthStep, false);		//	����True�ڴ�ѭ���ﻻ���ܶ�
	if (Matrix->Data == NULL) 
	{
		FreeMemory(Matrix);
		return NULL;
	}
	return Matrix;
}

/// <summary>
/// �ͷŴ����ľ���
/// </summary>
/// <param name="Matrix">��Ҫ�ͷŵľ������</param>
/// <returns>����ֵΪ0��ʾ�ɹ���������ʾʧ�ܡ�</returns>
void FreeMatrix(TMatrix *Matrix)
{
	if (Matrix == NULL) return;
	if (Matrix->Data == NULL) 
		FreeMemory(Matrix);
	else
	{
		FreeMemory(Matrix->Data);			//	ע���ͷŵ�˳��
		FreeMemory(Matrix);
	}
}

/// <summary>
/// ��¡���еľ���
/// </summary>
/// <param name="Src">����¡�ľ������</param>
/// <param name="Dest">��¡��ľ������</param>
TMatrix *Clone (TMatrix *Src)
{
	if (Src == NULL || Src->Data == NULL) return NULL;
	TMatrix *Dest = CreateMatrix(Src->Width, Src->Height, Src->Depth, Src->Channel, Src->RowAligned);
	if (Dest != NULL)	
	{
		memcpy(Dest->Data, Src->Data, Src->Height * Src->WidthStep);
		return Dest;
	}
	return NULL;	
}

TMatrix *DividMatrix(TMatrix *Src, TMatrix *Src2)
{
	
	int W = Src->Width;
	int H = Src->Height;
	int nStep = Src->WidthStep;
	int nStep2 = Src2->WidthStep;
	TMatrix * dst = CreateMatrix(W, H, Src->Depth, Src->Channel, false);
	div_Operate(Src->Data, nStep, Src2->Data, nStep2,
		dst->Data, dst->WidthStep, W, H, 1.0);
	return dst;
}

