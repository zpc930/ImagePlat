#include "stdafx.h"
#include "Imgresize.h"
#include "arithm.h"
#include <algorithm>
static const int MAX_ESIZE = 16;
const int INTER_RESIZE_COEF_BITS = 11;
const int INTER_RESIZE_COEF_SCALE = 1 << INTER_RESIZE_COEF_BITS;

const int INTER_REMAP_COEF_BITS = 15;
const int INTER_REMAP_COEF_SCALE = 1 << INTER_REMAP_COEF_BITS;
void resize(TMatrix* img, TMatrix* pdsimg, int interpolation)
{
	TSize dsize(img->Width, img->Height);
	TSize ssize(pdsimg->Width, pdsimg->Height);
	double  inv_scale_x = (double)dsize.Width / ssize.Width;
	double  inv_scale_y = (double)dsize.Height / ssize.Height;
	int depth = img->Depth;
	int cn =img->Channel;
	double scale_x = 1. / inv_scale_x;
	double scale_y = 1. / inv_scale_y;
	int iscale_x = saturate_cast<int>(scale_x);
	int iscale_y = saturate_cast<int>(scale_y);
	int xmin = 0, xmax = dsize.Width, width = dsize.Width*cn;
	int  ksize = 2;
	if (interpolation == INTER_LINEAR)
	{
		ksize = 2;
	}
	int ksize2 = ksize / 2;
	uchar* _buffer = (uchar*)malloc((width + dsize.Height)*(sizeof(int)+sizeof(float)*ksize));
	int* xofs = (int*)(uchar*)_buffer;
	int* yofs = xofs + width;
	float* alpha = (float*)(yofs + dsize.Height);
	short* ialpha = (short*)alpha;
	float* beta = alpha + width*ksize;
	short* ibeta = ialpha + width*ksize;
	bool fixpt = depth == DEPTH_8U;
	float cbuf[MAX_ESIZE];	
	float fx, fy;
	int k, sx, sy, dx, dy;
	for (dx = 0; dx < dsize.Width; dx++)
	{
		fx = (float)((dx + 0.5)*scale_x - 0.5);
		sx = static_cast<int>(fx + 0.5);
		fx -= sx;

		if (sx < ksize2 - 1)
		{
			xmin = dx + 1;
			if (sx < 0 && (interpolation != INTER_CUBIC && interpolation != INTER_LANCZOS4))
				fx = 0, sx = 0;
		}

		if (sx + ksize2 >= ssize.Width)
		{
			xmax = min(xmax, dx);
			if (sx >= ssize.Width - 1 && (interpolation != INTER_CUBIC && interpolation != INTER_LANCZOS4))
				fx = 0, sx = ssize.Width - 1;
		}

		for (k = 0, sx *= cn; k < cn; k++)
			xofs[dx*cn + k] = sx + k;
		cbuf[0] = 1.f - fx;
		cbuf[1] = fx;
		if (fixpt)
		{
			for (k = 0; k < ksize; k++)
				ialpha[dx*cn*ksize + k] = saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
			for (; k < cn*ksize; k++)
				ialpha[dx*cn*ksize + k] = ialpha[dx*cn*ksize + k - ksize];
		}
		else
		{
			for (k = 0; k < ksize; k++)
				alpha[dx*cn*ksize + k] = cbuf[k];
			for (; k < cn*ksize; k++)
				alpha[dx*cn*ksize + k] = alpha[dx*cn*ksize + k - ksize];
		}
	}
}