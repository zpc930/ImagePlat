#include "stdafx.h"
#include "ImageUtility.h"

typedef __int64 int64;
typedef unsigned char uchar;
typedef unsigned short ushort;

namespace cvplat
{
	template<typename T> static void
		split_(const T* src, T** dst, int len, int cn)
	{
		int k = cn % 4 ? cn % 4 : 4;
		int i, j;
		if (k == 1)
		{
			T* dst0 = dst[0];
			for (i = j = 0; i < len; i++, j += cn)
				dst0[i] = src[j];
		}
		else if (k == 2)
		{
			T *dst0 = dst[0], *dst1 = dst[1];
			for (i = j = 0; i < len; i++, j += cn)
			{
				dst0[i] = src[j];
				dst1[i] = src[j + 1];
			}
		}
		else if (k == 3)
		{
			T *dst0 = dst[0], *dst1 = dst[1], *dst2 = dst[2];
			for (i = j = 0; i < len; i++, j += cn)
			{
				dst0[i] = src[j];
				dst1[i] = src[j + 1];
				dst2[i] = src[j + 2];
			}
		}
		else
		{
			T *dst0 = dst[0], *dst1 = dst[1], *dst2 = dst[2], *dst3 = dst[3];
			for (i = j = 0; i < len; i++, j += cn)
			{
				dst0[i] = src[j]; dst1[i] = src[j + 1];
				dst2[i] = src[j + 2]; dst3[i] = src[j + 3];
			}
		}

		for (; k < cn; k += 4)
		{
			T *dst0 = dst[k], *dst1 = dst[k + 1], *dst2 = dst[k + 2], *dst3 = dst[k + 3];
			for (i = 0, j = k; i < len; i++, j += cn)
			{
				dst0[i] = src[j]; dst1[i] = src[j + 1];
				dst2[i] = src[j + 2]; dst3[i] = src[j + 3];
			}
		}
	}

	template<typename T> static void
		merge_(const T** src, T* dst, int len, int cn)
	{
		int k = cn % 4 ? cn % 4 : 4;
		int i, j;
		if (k == 1)
		{
			const T* src0 = src[0];
			for (i = j = 0; i < len; i++, j += cn)
				dst[j] = src0[i];
		}
		else if (k == 2)
		{
			const T *src0 = src[0], *src1 = src[1];
			for (i = j = 0; i < len; i++, j += cn)
			{
				dst[j] = src0[i];
				dst[j + 1] = src1[i];
			}
		}
		else if (k == 3)
		{
			const T *src0 = src[0], *src1 = src[1], *src2 = src[2];
			for (i = j = 0; i < len; i++, j += cn)
			{
				dst[j] = src0[i];
				dst[j + 1] = src1[i];
				dst[j + 2] = src2[i];
			}
		}
		else
		{
			const T *src0 = src[0], *src1 = src[1], *src2 = src[2], *src3 = src[3];
			for (i = j = 0; i < len; i++, j += cn)
			{
				dst[j] = src0[i]; dst[j + 1] = src1[i];
				dst[j + 2] = src2[i]; dst[j + 3] = src3[i];
			}
		}

		for (; k < cn; k += 4)
		{
			const T *src0 = src[k], *src1 = src[k + 1], *src2 = src[k + 2], *src3 = src[k + 3];
			for (i = 0, j = k; i < len; i++, j += cn)
			{
				dst[j] = src0[i]; dst[j + 1] = src1[i];
				dst[j + 2] = src2[i]; dst[j + 3] = src3[i];
			}
		}
	}

	void split8u(const uchar* src, uchar** dst, int len, int cn)
	{
		
			split_(src, dst, len, cn);
	}

	void split16u(const ushort* src, ushort** dst, int len, int cn)
	{
		
			split_(src, dst, len, cn);
	}

	void split32s(const int* src, int** dst, int len, int cn)
	{
		
			split_(src, dst, len, cn);
	}

	void split64s(const int64* src, int64** dst, int len, int cn)
	{	
			split_(src, dst, len, cn);
	}
}