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