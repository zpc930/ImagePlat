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
TMatrix *CreateMatrix(int Width, int Height, int Depth, int Channel, int RowAligned);		//	创建数据矩阵
TMatrix *CloneMatrix(TMatrix *Src);															//	克隆数据矩阵
void FreeMatrix(TMatrix *Matrix);															//	释放数据矩阵
void *AllocMemory(unsigned int Size, bool ZeroMemory = false);								//	分配内存，32字节对齐
void FreeMemory(void *Ptr);																	//	释放内存
int GetElementSize(int Depth);																//	获取元素大小