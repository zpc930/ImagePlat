#pragma once
enum EdgeMode
{
	Tile = 0,				//	�ظ���Ե����
	Smear = 1				//	��������
};
enum DEPTH
{
	DEPTH_8U, 				//	unsigned char
	DEPTH_8S, 				//	char
	DEPTH_16U, 				//	char
	DEPTH_16S,				//	short
	DEPTH_32S,				//  int
	DEPTH_32F,				//	float
	DEPTH_64F				//	double
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
enum InterpolationFlags{
	/** nearest neighbor interpolation */
	INTER_NEAREST = 0,
	/** bilinear interpolation */
	INTER_LINEAR = 1,
	/** bicubic interpolation */
	INTER_CUBIC = 2,
	/** resampling using pixel area relation. It may be a preferred method for image decimation, as
	it gives moire'-free results. But when the image is zoomed, it is similar to the INTER_NEAREST
	method. */
	INTER_AREA = 3,
	/** Lanczos interpolation over 8x8 neighborhood */
	INTER_LANCZOS4 = 4,
	/** mask for interpolation codes */
	INTER_MAX = 7,
	/** flag, fills all of the destination image pixels. If some of them correspond to outliers in the
	source image, they are set to zero */
	WARP_FILL_OUTLIERS = 8,
	/** flag, inverse transformation

	For example, polar transforms:
	- flag is __not__ set: \f$dst( \phi , \rho ) = src(x,y)\f$
	- flag is set: \f$dst(x,y) = src( \phi , \rho )\f$
	*/
	WARP_INVERSE_MAP = 16
};
class TSize
{
public:
	TSize()
	{
		Width = Height = 0;
	}

	TSize(IN const TSize& size)
	{
		Width = size.Width;
		Height = size.Height;
	}

	TSize(IN INT width,
		IN INT height)
	{
		Width = width;
		Height = height;
	}

	TSize operator+(IN const TSize& sz) const
	{
		return TSize(Width + sz.Width,
			Height + sz.Height);
	}

	TSize operator-(IN const TSize& sz) const
	{
		return TSize(Width - sz.Width,
			Height - sz.Height);
	}

	BOOL Equals(IN const TSize& sz) const
	{
		return (Width == sz.Width) && (Height == sz.Height);
	}

	BOOL Empty() const
	{
		return (Width == 0 && Height == 0);
	}

public:

	INT Width;
	INT Height;
};