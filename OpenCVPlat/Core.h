#pragma once
#include "StdAfx.h"
#include "CommonHeader.h"
enum RETVAL
{
	RET_OK,
	RET_ERR_OUTOFMEMORY,
	RET_ERR_NULLREFERENCE,
	RET_ERR_ARGUMENTOUTOFRANGE,
	RET_ERR_PARAMISMATCH,
	RET_ERR_DIVIDEBYZERO,
	RET_ERR_NOTSUPPORTED,
	RET_ERR_UNKNOWN
};



struct Complex
{
	float Real;
	float Imag;
};

#define WidthBytes(bytes) (((bytes * 8) + 31) / 32 * 4)
TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned);		//	�������ݾ���
TMatrix *CloneMatrix(TMatrix *Src);															//	��¡���ݾ���
void FreeMatrix(TMatrix *Matrix);															//	�ͷ����ݾ���
void *AllocMemory(unsigned int Size, bool ZeroMemory = false);								//	�����ڴ棬32�ֽڶ���
void FreeMemory(void *Ptr);																	//	�ͷ��ڴ�
int GetElementSize(int Depth);																//	��ȡԪ�ش�С
template<typename T> void MatrixFill(TMatrix *Src, T value)
{
	int nDepth = Src->Depth;
	int nChannel = Src->Channel;
	int nW = Src->Width;
	int nH = Src->Height;
	switch (nDepth)
	{
	case DEPTH_8U:
	{
		unsigned char u8value = value;
		memset(Src->Data, u8value, Src->Height * Src->WidthStep);
	}
	break;
	case DEPTH_8S:
	{
		char s8value = value;
		char* pData = (char*)Src->Data;
		memset(pData, s8value, Src->Height * Src->WidthStep);
	}
	break;
	case DEPTH_16S:
	{
		short s16value = value;
		short* pData = (short*)Src->Data;
		for (int i = 0; i < nH;i++)
		{
			pData = (short*)(Src->Data + i*Src->WidthStep);
			for (int j = 0; j < nW; j++)
			{
				pData[j] = s16value;
			}
		}

	}
	break;
	case DEPTH_32S:
	{
		int S32value = value;
		int* pData = (int*)Src->Data;
		for (int i = 0; i < nH;i++)
		{
			pData = (int*)(Src->Data + i*Src->WidthStep);
			for (int j = 0; j < nW; j++)
			{
				pData[j] = S32value;
			}
		}
	}
	break;
	case DEPTH_32F:
	{
		float f32value = value;
		float* pData = (float*)Src->Data;
		for (int i = 0; i < nH;i++)
		{
			pData = (float*)(Src->Data + i*Src->WidthStep);
			for (int j = 0; j < nW; j++)
			{
				pData[j] = f32value;
			}
		}
	}
	break;
	case DEPTH_64F:
	{
		double d32value = value;
		double* pData = (double*)Src->Data;		
			for (int i = 0; i < nH;i++)
			{
				pData = (double*)(Src->Data + i*Src->WidthStep);
				for (int j = 0; j < nW; j++)
				{
					pData[j] = d32value;
				}
			}

	}
	break;
	default:
		break;
	}
};
TMatrix *DividMatrix64F(TMatrix *Src, TMatrix *Src2);
TMatrix *MultiplyMatrix64F(TMatrix *Src, TMatrix *Src2);
TMatrix *SubMatrix64F(TMatrix *Src, TMatrix *Src2);
TMatrix *AddMatrix64F(TMatrix *Src, TMatrix *Src2);