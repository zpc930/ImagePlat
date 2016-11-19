#include "StdAfx.h"

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

enum DEPTH
{
	DEPTH_8U, 				//	unsigned char
	DEPTH_8S, 				//	char
	DEPTH_16S,				//	short
	DEPTH_32S,				//  int
	DEPTH_32F,				//	float
	DEPTH_64F				//	double
};

enum EdgeMode
{
	Tile = 0,				//	�ظ���Ե����
	Smear = 1				//	��������
};

struct TMatrix
{
	int Width;				//	����Ŀ��
	int Height;				//	����ĸ߶�
	int Channel;			//	����ͨ����
	int Depth;				//	����Ԫ�ص�����
	int WidthStep;			//	����һ��Ԫ�ص�ռ�õ��ֽ���
	int RowAligned;			//	�Ƿ�ʹ�����ֽڶ���
	unsigned char *Data;	//	���������
};

struct Complex
{
	float Real;
	float Imag;
};
union Cv32suf
{
	int i;
	unsigned u;
	float f;
};
#define WidthBytes(bytes) (((bytes * 8) + 31) / 32 * 4)
TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned);		//	�������ݾ���
TMatrix *CloneMatrix(TMatrix *Src);															//	��¡���ݾ���
void FreeMatrix(TMatrix *Matrix);															//	�ͷ����ݾ���
void *AllocMemory(unsigned int Size, bool ZeroMemory = false);								//	�����ڴ棬32�ֽڶ���
void FreeMemory(void *Ptr);																	//	�ͷ��ڴ�
int GetElementSize(int Depth);																//	��ȡԪ�ش�С