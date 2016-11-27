#pragma once
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef signed char schar;
template<typename _Tp> static inline _Tp saturate_cast(uchar v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(schar v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(ushort v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(short v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(unsigned v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(int v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(float v) { return _Tp(v); }
template<typename _Tp> static inline _Tp saturate_cast(double v) { return _Tp(v); }

template<typename T1, typename T2 = T1, typename T3 = T1> struct OpAdd
{
	typedef T1 type1;
	typedef T2 type2;
	typedef T3 rtype;
	T3 operator ()(const T1 a, const T2 b) const { return saturate_cast<T3>(a + b); }
};

template<typename T1, typename T2 = T1, typename T3 = T1> struct OpSub
{
	typedef T1 type1;
	typedef T2 type2;
	typedef T3 rtype;
	T3 operator ()(const T1 a, const T2 b) const { return saturate_cast<T3>(a - b); }
};

template<typename T> static void
div_Operate(const T* src1, int step1, const T* src2, int step2,
T* dst, int step, int width,int height , double scale)
{
	step1 /= sizeof(src1[0]);
	step2 /= sizeof(src2[0]);
	step /= sizeof(dst[0]);

	for (; height--; src1 += step1, src2 += step2, dst += step)
	{
		int i = 0;
		for (; i <= width - 4; i += 4)
		{
			if (src2[i] != 0 && src2[i + 1] != 0 && src2[i + 2] != 0 && src2[i + 3] != 0)
			{
				double a = (double)src2[i] * src2[i + 1];
				double b = (double)src2[i + 2] * src2[i + 3];
				double d = scale / (a * b);
				b *= d;
				a *= d;

				T z0 = saturate_cast<T>(src2[i + 1] * ((double)src1[i] * b));
				T z1 = saturate_cast<T>(src2[i] * ((double)src1[i + 1] * b));
				T z2 = saturate_cast<T>(src2[i + 3] * ((double)src1[i + 2] * a));
				T z3 = saturate_cast<T>(src2[i + 2] * ((double)src1[i + 3] * a));

				dst[i] = z0; dst[i + 1] = z1;
				dst[i + 2] = z2; dst[i + 3] = z3;
			}
			else
			{
				T z0 = src2[i] != 0 ? saturate_cast<T>(src1[i] * scale / src2[i]) : 0;
				T z1 = src2[i + 1] != 0 ? saturate_cast<T>(src1[i + 1] * scale / src2[i + 1]) : 0;
				T z2 = src2[i + 2] != 0 ? saturate_cast<T>(src1[i + 2] * scale / src2[i + 2]) : 0;
				T z3 = src2[i + 3] != 0 ? saturate_cast<T>(src1[i + 3] * scale / src2[i + 3]) : 0;

				dst[i] = z0; dst[i + 1] = z1;
				dst[i + 2] = z2; dst[i + 3] = z3;
			}
		}

		for (; i < width; i++)
			dst[i] = src2[i] != 0 ? saturate_cast<T>(src1[i] * scale / src2[i]) : 0;
	}
}
template<typename T, typename WT> static void
mul_operate(const T* src1, size_t step1, const T* src2, size_t step2,
T* dst, size_t step, int width,int height , WT scale)
{
	step1 /= sizeof(src1[0]);
	step2 /= sizeof(src2[0]);
	step /= sizeof(dst[0]);

	if (scale == (WT)1.)
	{
		for (; height--; src1 += step1, src2 += step2, dst += step)
		{
			int i = 0;

			for (; i <= width - 4; i += 4)
			{
				T t0;
				T t1;
				t0 = saturate_cast<T>(src1[i] * src2[i]);
				t1 = saturate_cast<T>(src1[i + 1] * src2[i + 1]);
				dst[i] = t0;
				dst[i + 1] = t1;

				t0 = saturate_cast<T>(src1[i + 2] * src2[i + 2]);
				t1 = saturate_cast<T>(src1[i + 3] * src2[i + 3]);
				dst[i + 2] = t0;
				dst[i + 3] = t1;
			}

			for (; i < width; i++)
				dst[i] = saturate_cast<T>(src1[i] * src2[i]);
		}
	}
	else
	{
		for (; height--; src1 += step1, src2 += step2, dst += step)
		{
			int i = 0;

			for (; i <= width - 4; i += 4)
			{
				T t0 = saturate_cast<T>(scale*(WT)src1[i] * src2[i]);
				T t1 = saturate_cast<T>(scale*(WT)src1[i + 1] * src2[i + 1]);
				dst[i] = t0; dst[i + 1] = t1;

				t0 = saturate_cast<T>(scale*(WT)src1[i + 2] * src2[i + 2]);
				t1 = saturate_cast<T>(scale*(WT)src1[i + 3] * src2[i + 3]);
				dst[i + 2] = t0; dst[i + 3] = t1;
			}

			for (; i < width; i++)
				dst[i] = saturate_cast<T>(scale*(WT)src1[i] * src2[i]);
		}
	}
}

template<typename T, class Op>
void vBinOp8(const T* src1, size_t step1, const T* src2, size_t step2, T* dst, size_t step, int width, int height)
{
Op op;

	for (; height--; src1 += step1 / sizeof(src1[0]),
		src2 += step2 / sizeof(src2[0]),
		dst += step / sizeof(dst[0]))
	{
		int x = 0;
		for (; x <= width - 4; x += 4)
		{
			T v0 = op(src1[x], src2[x]);
			T v1 = op(src1[x + 1], src2[x + 1]);
			dst[x] = v0; dst[x + 1] = v1;
			v0 = op(src1[x + 2], src2[x + 2]);
			v1 = op(src1[x + 3], src2[x + 3]);
			dst[x + 2] = v0; dst[x + 3] = v1;
		}

		for (; x < width; x++)
			dst[x] = op(src1[x], src2[x]);
	}
}

template<typename T, class Op>
void vBinOp16(const T* src1, size_t step1, const T* src2, size_t step2,
	T* dst, size_t step, int width, int height)
{

	Op op;

	for (; height--; src1 += step1 / sizeof(src1[0]),
		src2 += step2 / sizeof(src2[0]),
		dst += step / sizeof(dst[0]))
	{
		int x = 0;
			for (; x <= width - 4; x += 4)
			{
				T v0 = op(src1[x], src2[x]);
				T v1 = op(src1[x + 1], src2[x + 1]);
				dst[x] = v0; dst[x + 1] = v1;
				v0 = op(src1[x + 2], src2[x + 2]);
				v1 = op(src1[x + 3], src2[x + 3]);
				dst[x + 2] = v0; dst[x + 3] = v1;
			}

		for (; x < width; x++)
			dst[x] = op(src1[x], src2[x]);
	}
}


template<class Op>
void vBinOp32s(const int* src1, size_t step1, const int* src2, size_t step2,
	int* dst, size_t step, int width, int height)
{

	Op op;

	for (; height--; src1 += step1 / sizeof(src1[0]),
		src2 += step2 / sizeof(src2[0]),
		dst += step / sizeof(dst[0]))
	{
		int x = 0;
		for (; x <= width - 4; x += 4)
		{
			int v0 = op(src1[x], src2[x]);
			int v1 = op(src1[x + 1], src2[x + 1]);
			dst[x] = v0; dst[x + 1] = v1;
			v0 = op(src1[x + 2], src2[x + 2]);
			v1 = op(src1[x + 3], src2[x + 3]);
			dst[x + 2] = v0; dst[x + 3] = v1;
		}

		for (; x < width; x++)
			dst[x] = op(src1[x], src2[x]);
	}
}


template<class Op, class Op32>
void vBinOp32f(const float* src1, size_t step1, const float* src2, size_t step2,
	float* dst, size_t step, int width, int height)
{

	Op op;

	for (; height--; src1 += step1 / sizeof(src1[0]),
		src2 += step2 / sizeof(src2[0]),
		dst += step / sizeof(dst[0]))
	{
		int x = 0;


#if CV_ENABLE_UNROLLED
		for (; x <= width - 4; x += 4)
		{
			float v0 = op(src1[x], src2[x]);
			float v1 = op(src1[x + 1], src2[x + 1]);
			dst[x] = v0; dst[x + 1] = v1;
			v0 = op(src1[x + 2], src2[x + 2]);
			v1 = op(src1[x + 3], src2[x + 3]);
			dst[x + 2] = v0; dst[x + 3] = v1;
		}
#endif
		for (; x < width; x++)
			dst[x] = op(src1[x], src2[x]);
	}
}
template<class Op >
void vBinOp64f(const double* src1, size_t step1, const double* src2, size_t step2,
	double* dst, size_t step, int width, int height)
{

	Op op;

	for (; height--; src1 += step1 / sizeof(src1[0]),
		src2 += step2 / sizeof(src2[0]),
		dst += step / sizeof(dst[0]))
	{
		int x = 0;


			for (; x <= width - 4; x += 4)
			{
				double v0 = op(src1[x], src2[x]);
				double v1 = op(src1[x + 1], src2[x + 1]);
				dst[x] = v0; dst[x + 1] = v1;
				v0 = op(src1[x + 2], src2[x + 2]);
				v1 = op(src1[x + 3], src2[x + 3]);
				dst[x + 2] = v0; dst[x + 3] = v1;
			}

		for (; x < width; x++)
			dst[x] = op(src1[x], src2[x]);
	}
}