#include "stdafx.h"
#include "HistogramCalc.h"



#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)
CHistogramCalc::CHistogramCalc() :
m_pixelCount(0)
{
}


CHistogramCalc::~CHistogramCalc()
{
}

void CHistogramCalc::calculateHistogram(const cv::Mat& f_image)
{
	m_pixelCount = f_image.size().height* f_image.size().width;

	m_channel0.m_xvalues.clear();
	m_channel0.m_yvalues.clear();

	m_channel1.m_xvalues.clear();
	m_channel1.m_yvalues.clear();

	m_channel2.m_xvalues.clear();
	m_channel2.m_yvalues.clear();

	cv::Mat l_image = f_image;

	cv::vector<cv::Mat> bgr_planes;
	cv::split(f_image, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	cv::Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

	if (CV_8UC1 != l_image.type())
	{
		cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	}

	for (int l_count = 0; l_count < histSize; l_count++)
	{
		m_channel0.m_xvalues.push_back(l_count);
		m_channel0.m_yvalues.push_back(b_hist.at<float>(l_count));

		if (CV_8UC1 != l_image.type())
		{
			m_channel1.m_xvalues.push_back(l_count);
			m_channel1.m_yvalues.push_back(g_hist.at<float>(l_count));
			m_channel2.m_xvalues.push_back(l_count);
			m_channel2.m_yvalues.push_back(r_hist.at<float>(l_count));
		}
	}
}
	
int32_t CHistogramCalc::Histogram(const cv::Mat& f_image, int32_t* red, int32_t* green, int32_t* blue, int32_t* gray, int32_t colorspace)
{
	RGBQUAD color;

	if (red) memset(red, 0, 256 * sizeof(int32_t));
	if (green) memset(green, 0, 256 * sizeof(int32_t));
	if (blue) memset(blue, 0, 256 * sizeof(int32_t));
	if (gray) memset(gray, 0, 256 * sizeof(int32_t));

	int32_t xmin, xmax, ymin, ymax;
	xmin = ymin = 0;
	xmax = f_image.size().width; ymax = f_image.size().height;


	for (int32_t y = ymin; y < ymax; y++){
		uchar *old_data = (uchar *)f_image.ptr<uchar>(y);
		for (int32_t x = xmin; x < xmax; x++){
			color.rgbBlue = old_data[x * 3];
			color.rgbGreen = old_data[x * 3 + 1];
			color.rgbRed = old_data[x * 3 + 2];
			if (red) red[color.rgbRed]++;
			if (green) green[color.rgbGreen]++;
			if (blue) blue[color.rgbBlue]++;
			if (gray) gray[(uint8_t)RGB2GRAY(color.rgbRed, color.rgbGreen, color.rgbBlue)]++;
		}
	}

	m_channel0.m_xvalues.clear();
	m_channel0.m_yvalues.clear();

	m_channel1.m_xvalues.clear();
	m_channel1.m_yvalues.clear();

	m_channel2.m_xvalues.clear();
	m_channel2.m_yvalues.clear();
	int32_t n = 0;
	for (int32_t i = 0; i < 256; i++){
		m_channel0.m_xvalues.push_back(i);
		m_channel0.m_yvalues.push_back(red[i]);
		m_channel1.m_xvalues.push_back(i);
		m_channel1.m_yvalues.push_back(green[i]);
		m_channel2.m_xvalues.push_back(i);
		m_channel2.m_yvalues.push_back(blue[i]);
		
	}
	return 0;
}
int32_t CHistogramCalc::InteEqualize(cv::Mat& f_image)
{
	int32_t xmin, xmax, ymin, ymax;
	xmin = ymin = 0;
	xmax = f_image.size().width; ymax = f_image.size().height;
	int size = ymax *xmax;
	float  fpHist[3][256];
	float eqHistTemp[3][256];
	int eqHist[3][256];
	for (int i = 0; i < 256; i++)   // 计算灰度分布密度
	{
		fpHist[0][i] = (float)m_channel0.m_yvalues[i] / (float)size;
		fpHist[1][i] = (float)m_channel1.m_yvalues[i] / (float)size;
		fpHist[2][i] = (float)m_channel2.m_yvalues[i] / (float)size;
	}
	for (int i = 0; i < 256; i++)   // 计算累计直方图分布
	{
		if (i == 0)
		{
			eqHistTemp[0][i] = fpHist[0][i];
			eqHistTemp[1][i] = fpHist[1][i];
			eqHistTemp[2][i] = fpHist[2][i];
		}
		else
		{
			eqHistTemp[0][i] = eqHistTemp[0][i - 1] + fpHist[0][i];
			eqHistTemp[1][i] = eqHistTemp[1][i - 1] + fpHist[1][i];
			eqHistTemp[2][i] = eqHistTemp[2][i - 1] + fpHist[2][i];
		}
	}
	//累计分布取整，保存计算出来的灰度映射关系
	for (int i = 0; i < 256; i++)
	{
		eqHist[0][i] = (int)(255.0 * eqHistTemp[0][i] + 0.5);
		eqHist[1][i] = (int)(255.0 * eqHistTemp[1][i] + 0.5);
		eqHist[2][i] = (int)(255.0 * eqHistTemp[2][i] + 0.5);
	}
	for (int i = 0; i < ymax; i++) //进行灰度映射 均衡化
	{
		uchar *data = f_image.ptr<uchar>(i);
		for (int j = 0; j < xmax; j++)
		{
			int bn = data[j * 3] ;
			int gn = data[j * 3 + 1];
			int rn = data[j * 3 + 2];
			data[j * 3] = eqHist[0][bn];
			data[j * 3 + 1] = eqHist[1][gn];
			data[j * 3 + 2] = eqHist[2][rn];
		}
	}
	return 0;
}
#define MAXBIN 256
int32_t CHistogramCalc::AHE(cv::Mat& f_image, int nNumber)
{
	if (nNumber < 1)	nNumber		= 1;
	float Histo[3][MAXBIN];
	int Xstart = 0;
	int Xend = 0;
	int Ystart = 0;
	int Yend = 0;
	float w1, w2, w3, w4, w5;
	int Xdim = f_image.size().width;
	int Ydim = f_image.size().height;
	float Xsize = Xdim / (float)nNumber;
	float Ysize = Ydim /(float) nNumber;
	float*** Map[3];
	cv::Mat dst = f_image.clone();
	for (int i = 0; i < 3;i++)
	{
		Map[i] =(float***) malloc(MAXBIN*sizeof(float**));
		for (int j = 0; j < MAXBIN; j++)
		{
			Map[i][j] = (float**)malloc(nNumber*sizeof(float*));
			for (int k = 0; k < nNumber;k++)
			{
				Map[i][j][k] = (float*)malloc(nNumber*sizeof(float));
			}
		}
	}
	for (int r = 0; r < nNumber;r++)
	{
		for (int c = 0; c < nNumber;c++)
		{
			// Calculate each partition of the iamge
			Xstart = (int)(c*Xsize);
			Xend = (int)((c + 1)*Xsize);
			if (Xend >= Xdim) Xend = Xdim;
			Ystart = (int)(r*Ysize);
			Yend = (int)((r+1)*Ysize);
			if (Yend >= Ydim) Yend = Ydim;
			/*Calcaulate bounding rectangle for patch*/
			memset(Histo,0,sizeof(float)*MAXBIN*3);
			for (int y = Ystart; y < Yend;y++)
			{
				uchar *old_data = (uchar *)f_image.ptr<uchar>(y);
				for (int x = Xstart; x<Xend; x++)
				{
					RGBQUAD color;
					color.rgbRed = f_image.at<cv::Vec3b>(y, x)[0];
					color.rgbGreen = f_image.at<cv::Vec3b>(y, x)[1];
					color.rgbBlue = f_image.at<cv::Vec3b>(y, x)[2];
					Histo[0][color.rgbRed]++;
					Histo[1][color.rgbGreen]++;
					Histo[2][color.rgbBlue]++;
				}
			}
			for (int i = 1; i < MAXBIN; i++)
			{
				Histo[0][i] = Histo[0][i] + Histo[0][i - 1];
				Histo[1][i] = Histo[1][i] + Histo[1][i - 1];
				Histo[2][i] = Histo[2][i] + Histo[2][i - 1];
			}
			for (int k = 0; k < 3; k++)
			{
				float Scale = 255.0 / (1 + Histo[k][MAXBIN - 1]);
				for (int i = 0; i < MAXBIN; i++)
					Map[k][i][r][c] = Histo[k][i] * Scale;
			}
		}
		
		
	}
	/* Process internal partitions of the image */
	for (int r = 0; r < (nNumber - 1); r++)
		for (int c = 0; c < (nNumber - 1); c++)
		{
			/* Calculate bounding rectangle for patch */
			Xstart = (int)(c*Xsize + Xsize / 2);
			Xend = (int)((c + 1)*Xsize + Xsize / 2);
			Ystart = (int)(r*Ysize + Ysize / 2);
			Yend = (int)((r + 1)*Ysize + Ysize / 2);
			/* Peform interpolated histogram equalization */
			for (int y = Ystart; y < Yend; y++)
				for (int x = Xstart; x < Xend; x++)
				{
					w1 = (Xend - 1 - x)*(Yend - 1 - y);
					w2 = (x - Xstart)*(Yend - 1 - y);
					w3 = (Xend - 1 - x)*(y - Ystart);
					w4 = (x - Xstart)*(y - Ystart);
					w5 = w1 + w2 + w3 + w4;
					for (int ii = 0; ii < 3;ii++)
					{
						dst.at<cv::Vec3b>(y, x)[ii] = (w1 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r][c]
							+ w2 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r][c + 1]
							+ w3 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r + 1][c]
							+ w4 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r + 1][c + 1]) / w5;
					}
				}
		}
	/* Process four corner partitions of the image */
	Xstart = (int)(Xsize / 2);
	Ystart = (int)(Ysize / 2);
	Xend = (int)((nNumber - 1)*Xsize + Xsize / 2);
	Yend = (int)((nNumber - 1)*Ysize + Ysize / 2);
	for (int y = 0; y < Ystart; y++)
		for (int x = 0; x < Xstart; x++)
			for (int ii = 0; ii < 3; ii++)
				dst.at<cv::Vec3b>(y,x)[ii]  =Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][0][0];
	for (int y = Yend; y < Ydim; y++)
		for (int x = 0; x < Xstart; x++)
			for (int ii = 0; ii < 3; ii++)
				dst.at<cv::Vec3b>(y,x)[ii] = Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][nNumber - 1][0];
	for (int y = 0; y < Ystart; y++)
		for (int x = Xend; x < Xdim; x++)
			for (int ii = 0; ii < 3; ii++)
				dst.at<cv::Vec3b>(y,x)[ii] = Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][0][nNumber - 1];
	for (int y = Yend; y < Ydim; y++)
		for (int x = Xend; x < Xdim; x++)
			for (int ii = 0; ii < 3; ii++)
				dst.at<cv::Vec3b>(y,x)[ii] = Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][nNumber - 1][nNumber - 1];

	/* Process top and bottom partitions of the image */
	for (int c = 0; c < (nNumber - 1); c++)
	{
		Xstart = (int)(c*Xsize + Xsize / 2);
		Xend = (int)((c + 1)*Xsize + Xsize / 2);
		
		for (int x = Xstart; x < Xend; x++)
		{
			w1 = Xend - 1 - x;
			w2 = x - Xstart;
			w3 = w1 + w2;

			/* Handle top */
			Ystart = 0;
			Yend = (int)(Ysize / 2);
			for (int y = Ystart; y < Yend; y++)
				for (int ii = 0; ii < 3; ii++)				
					dst.at<cv::Vec3b>(y,x)[ii] = (w1 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][0][c]
					+ w2 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][0][c + 1]) / w3;

			/* Handle bottom */
			Ystart = (int)((nNumber - 1)*Ysize + Ysize / 2);
			Yend = Ydim;
			for (int y = Ystart; y < Yend; y++)
				for (int ii = 0; ii < 3; ii++)
					dst.at<cv::Vec3b>(y,x)[ii] = (w1 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][nNumber - 1][c]
					+ w2 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][nNumber - 1][c + 1]) / w3;
		}
	}
	/* Process left and right partitions of the image */
	for (int r = 0; r < (nNumber - 1); r++)
	{
		Ystart = (int)(r*Ysize + Ysize / 2);
		Yend = (int)((r + 1)*Ysize + Ysize / 2);
		
		for (int y = Ystart; y < Yend; y++)
		{
			w1 = Yend - 1 - y;
			w2 = y - Ystart;
			w3 = w1 + w2;

			/* Handle left */
			Xstart = 0;
			Xend = (int)(Xsize / 2);
			for (int x = Xstart; x < Xend; x++)
				for (int ii = 0; ii < 3; ii++)
					dst.at<cv::Vec3b>(y,x)[ii] = (w1 *Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r][0]
					+ w2 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r + 1][0]) / w3;

			/* Handle right */
			Xstart = (int)((nNumber - 1)*Xsize + Xsize / 2);
			Xend = Xdim;
			for (int x = Xstart; x < Xend; x++)
				for (int ii = 0; ii < 3; ii++)
					dst.at<cv::Vec3b>(y,x)[ii] = (w1 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r][nNumber - 1]
					+ w2 * Map[ii][f_image.at<cv::Vec3b>(y,x)[ii]][r + 1][nNumber - 1]) / w3;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		
		for (int j = 0; j < MAXBIN; j++)
		{
			
			for (int k = 0; k < nNumber; k++)
			{
				free(Map[i][j][k]);
			}
			free(Map[i][j]);
		}
		free(Map[i]);
	}
	dst.assignTo(f_image);
	dst.release();
	return 0;
}

#define BYTE_IMAGE

#ifdef BYTE_IMAGE
typedef unsigned char kz_pixel_t;     /* for 8 bit-per-pixel images */
#define uiNR_OF_GREY (256)
#else
typedef unsigned short kz_pixel_t;     /* for 12 bit-per-pixel images (default) */
# define uiNR_OF_GREY (4096)
#endif
const unsigned int uiMAX_REG_X = 16;      /* max. # contextual regions in x-direction */
const unsigned int uiMAX_REG_Y = 16;      /* max. # contextual regions in y-direction */
int CLAHE(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes,
	kz_pixel_t Min, kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,
	unsigned int uiNrBins, float fCliplimit)
	/*   pImage - Pointer to the input/output image
	*   uiXRes - Image resolution in the X direction
	*   uiYRes - Image resolution in the Y direction
	*   Min - Minimum greyvalue of input image (also becomes minimum of output image)
	*   Max - Maximum greyvalue of input image (also becomes maximum of output image)
	*   uiNrX - Number of contextial regions in the X direction (min 2, max uiMAX_REG_X)
	*   uiNrY - Number of contextial regions in the Y direction (min 2, max uiMAX_REG_Y)
	*   uiNrBins - Number of greybins for histogram ("dynamic range")
	*   float fCliplimit - Normalized cliplimit (higher values give more contrast)
	* The number of "effective" greylevels in the output image is set by uiNrBins; selecting
	* a small value (eg. 128) speeds up processing and still produce an output image of
	* good quality. The output image will have the same minimum and maximum value as the input
	* image. A clip limit smaller than 1 results in standard (non-contrast limited) AHE.
	*/
{

	unsigned int uiX, uiY;          /* counters */
	unsigned int uiXSize, uiYSize, uiSubX, uiSubY; /* size of context. reg. and subimages */
	unsigned int uiXL, uiXR, uiYU, uiYB;  /* auxiliary variables interpolation routine */
	unsigned long ulClipLimit, ulNrPixels;/* clip limit and region pixel count */
	kz_pixel_t* pImPointer;           /* pointer to image */
	kz_pixel_t aLUT[uiNR_OF_GREY];        /* lookup table used for scaling of input image */
	unsigned long* pulHist, *pulMapArray; /* pointer to histogram and mappings*/
	unsigned long* pulLU, *pulLB, *pulRU, *pulRB; /* auxiliary pointers interpolation */

	if (uiNrX > uiMAX_REG_X) return -1;       /* # of regions x-direction too large */
	if (uiNrY > uiMAX_REG_Y) return -2;       /* # of regions y-direction too large */
	if (uiXRes % uiNrX) return -3;      /* x-resolution no multiple of uiNrX */
	if (uiYRes & uiNrY) return -4;      /* y-resolution no multiple of uiNrY */
	if (Max >= uiNR_OF_GREY) return -5;       /* maximum too large */
	if (Min >= Max) return -6;          /* minimum equal or larger than maximum */
	if (uiNrX < 2 || uiNrY < 2) return -7;/* at least 4 contextual regions required */
	if (fCliplimit == 1.0) return 0;      /* is OK, immediately returns original image. */
	if (uiNrBins == 0) uiNrBins = 128;      /* default value when not specified */

	pulMapArray = (unsigned long*)malloc(sizeof(unsigned long)*uiNrX*uiNrY*uiNrBins);
	if (pulMapArray == NULL)
		return -8;
	uiXSize = uiXRes / uiNrX; uiYSize = uiYRes / uiNrY; /* Actual size of contextual regions */
	ulNrPixels = (unsigned long)uiXSize* (unsigned long)uiYSize;
	if (fCliplimit > 0.0) {          /* Calculate actual cliplimit     */
		ulClipLimit = (unsigned long)(fCliplimit * (uiXSize * uiYSize) / uiNrBins);
		ulClipLimit = (ulClipLimit < 1UL) ? 1UL : ulClipLimit;
	}
	else ulClipLimit = 1UL << 14;          /* Large value, do not clip (AHE) */
	return 0;
}