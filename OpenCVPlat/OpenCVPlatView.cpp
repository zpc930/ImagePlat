
// OpenCVPlatView.cpp : COpenCVPlatView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "OpenCVPlat.h"
#endif
#include "HistogramCalc.h"

#include "OpenCVPlatDoc.h"
#include "OpenCVPlatView.h"
#include "DialogVignette.h"
#include "MedianFilter.h"
#include "GaussianBlur.h"
#include "ColorBalance.h"
#include "Saturation.h"
#include "Sharpen.h"
#include "Liquify.h"
#include "Skin.h"
#include "Lomo.h"
#include "Inpainting.h"
#include "retinex.h"
#include "GrayWorld.h"
#include "bilateralFilter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenCVPlatView

IMPLEMENT_DYNCREATE(COpenCVPlatView, CView)

BEGIN_MESSAGE_MAP(COpenCVPlatView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIGNETTE, &COpenCVPlatView::OnVignette)
	ON_COMMAND(ID_MEDIAN_FILTER, &COpenCVPlatView::OnMedianFilter)
	ON_COMMAND(ID_GAUSSIAN_BLUR, &COpenCVPlatView::OnGaussianBlur)
	ON_COMMAND(ID_COLOR_BALANCE, &COpenCVPlatView::OnColorBalance)
	ON_COMMAND(ID_SATURATION, &COpenCVPlatView::OnSaturation)
	ON_COMMAND(ID_SHARPEN, &COpenCVPlatView::OnSharpen)
	ON_COMMAND(ID_LIQUIFY, &COpenCVPlatView::OnLiquify)
	ON_COMMAND(ID_SKIN, &COpenCVPlatView::OnSkin)
	ON_COMMAND(ID_LOMO, &COpenCVPlatView::OnLomo)
	ON_COMMAND(ID_IMAGE_INPAINT, &COpenCVPlatView::OnImageInpaint)
	ON_COMMAND(ID_HISTOGRAM, &COpenCVPlatView::OnHistogram)
END_MESSAGE_MAP()

// COpenCVPlatView 构造/析构

COpenCVPlatView::COpenCVPlatView()
{
	// TODO:  在此处添加构造代码
	start_inpaint = false;
}

COpenCVPlatView::~COpenCVPlatView()
{
}

BOOL COpenCVPlatView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// COpenCVPlatView 绘制

void COpenCVPlatView::OnDraw(CDC* pDC)
{
	COpenCVPlatDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	cv::Mat &image = pDoc->image;
	if (image.cols > 0 && image.rows > 0)
	{
		RECT rect;
		GetClientRect(&rect);
		cv::Mat image_show(abs(rect.top - rect.bottom), abs(rect.right - rect.left), CV_8UC3);
		resize(image, image_show, image_show.size());

		ATL::CImage CI;
		int w = image_show.cols;//宽  
		int h = image_show.rows;//高  
		int channels = image_show.channels();//通道数  

		CI.Create(w, h, 8 * channels);
		if (channels == 1)
		{
			RGBQUAD* ColorTable;
			int MaxColors = 256;
			ColorTable = new RGBQUAD[MaxColors];
			CI.GetColorTable(0, MaxColors, ColorTable);
			for (int i = 0; i<MaxColors; i++)
			{
				ColorTable[i].rgbBlue = (BYTE)i;
				ColorTable[i].rgbGreen = (BYTE)i;
				ColorTable[i].rgbRed = (BYTE)i;
			}
			CI.SetColorTable(0, MaxColors, ColorTable);
			delete[]ColorTable;
		}

		uchar *pS;
		uchar *pImg = (uchar *)CI.GetBits(); 
		int step = CI.GetPitch();
		for (int i = 0; i < h; i++)
		{
			pS = image_show.ptr<uchar>(i);
			memcpy(pImg + i*step, pS, sizeof(uchar) * channels * w);
		}
		
		CI.Draw(*pDC, 0, 0);
		ReleaseDC(pDC);
		CI.Destroy();
		
	}
	if (start_inpaint)
	{
		inpaint();
	}
}


// COpenCVPlatView 打印

BOOL COpenCVPlatView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void COpenCVPlatView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void COpenCVPlatView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// COpenCVPlatView 诊断

#ifdef _DEBUG
void COpenCVPlatView::AssertValid() const
{
	CView::AssertValid();
}

void COpenCVPlatView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenCVPlatDoc* COpenCVPlatView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenCVPlatDoc)));
	return (COpenCVPlatDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenCVPlatView 消息处理程序


BOOL COpenCVPlatView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}


void COpenCVPlatView::OnVignette()
{
	// TODO:  在此添加命令处理程序代码
	CDialogVignette dialog_vignette;
	dialog_vignette.DoModal();
}


void COpenCVPlatView::OnMedianFilter()
{
	// TODO:  在此添加命令处理程序代码
	MedianFilter dialog_median_filter;
	dialog_median_filter.DoModal();
}


void COpenCVPlatView::OnGaussianBlur()
{
	// TODO:  在此添加命令处理程序代码
	CGaussianBlur dialog_gaussian_blur;
	dialog_gaussian_blur.DoModal();
}


void COpenCVPlatView::OnColorBalance()
{
	// TODO:  在此添加命令处理程序代码
	CColorBalance dialog_color_balance;
	dialog_color_balance.DoModal();
}


void COpenCVPlatView::OnSaturation()
{
	// TODO:  在此添加命令处理程序代码
	CSaturation dialog_saturation;
	dialog_saturation.DoModal();
}


void COpenCVPlatView::OnSharpen()
{
	// TODO:  在此添加命令处理程序代码
	CSharpen dialog_sharpen;
	dialog_sharpen.DoModal();
}


void COpenCVPlatView::OnLiquify()
{
	// TODO:  在此添加命令处理程序代码
	CLiquify dialog_liquify;
	dialog_liquify.DoModal();
}


void COpenCVPlatView::OnSkin()
{
	// TODO:  在此添加命令处理程序代码
	CSkin dialog_skin;
	dialog_skin.DoModal();
}


void COpenCVPlatView::OnLomo()
{
	// TODO:  在此添加命令处理程序代码
	CLomo dialog_lomo;
	dialog_lomo.DoModal();
}


void COpenCVPlatView::OnImageInpaint()
{
	// TODO:  在此添加命令处理程序代码
	COpenCVPlatDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	cv::Mat &image = pDoc->image;
	dst = image.clone();
	start_inpaint = true;
	Invalidate();
	//cv::normalize(image, image, 0, 255, NORM_MINMAX, CV_8UC1);
}

void COpenCVPlatView::inpaint()
{
	COpenCVPlatDoc* pDoc = GetDocument();
	cv::Mat &image = pDoc->image;
	int psize = 23;

	if (contains_hole(image))
	{
		initialize_hole_NNF(nnf, image, dst, psize, last_nnf);
		iterate_hole_NNF(nnf, psize, image, dst);
		reconstruct_from_hole_NNF_no_avg(nnf, image, dst, psize);
		last_nnf.release();
		last_nnf = nnf.clone();
	}
	else
	{
		initialize_hole_NNF(nnf, image, dst, psize, last_nnf);
		iterate_hole_NNF(nnf, psize, image, dst);
		reconstruct_from_hole_NNF(nnf, image, dst, 5);
		dst.release();
		nnf.release();
		last_nnf.release();

		start_inpaint = false;
	}
	Invalidate();
}


void pyramid_method(cv::Mat &image, int psize)
{
	cv::Mat srces[5];
	cv::Mat nnfs[5];
	cv::Mat old[5];
	cv::Mat temp;

	srces[4] = image.clone();
	for (int i = 4; i >= 1; i--)
		sample_down(srces[i], srces[i - 1]);
	for (int i = 0; i < 5; i++)
		old[i] = srces[i].clone();

	optimize_start(srces[0], 5);

	cv::resize(srces[0], temp, cv::Size(srces[1].cols, srces[1].rows));
	fill_image_with_image(srces[1], temp);

	for (int i = 1; i <= 3; i++)
	{
		initialize_NNF(nnfs[i], srces[i].cols, srces[i].rows);
		iterate_NNF(nnfs[i], psize, srces[i], old[i]);
		reconstruct_from_NNF_no_avg(nnfs[i], srces[i], old[i]);
		cv::resize(srces[i], temp, cv::Size(srces[i + 1].cols, srces[i + 1].rows));
		fill_image_with_image(srces[i + 1], temp);
	}

	initialize_NNF(nnfs[4], srces[4].cols, srces[4].rows);
	iterate_NNF(nnfs[4], psize, srces[4], old[4]);
	reconstruct_from_NNF_no_avg(nnfs[4], srces[4], old[4]);

	image.release();
	image = srces[4].clone();
	temp.release();
	for (int i = 0; i < 5; i++)
	{
		nnfs[i].release();
		srces[i].release();
		old[i].release();
	}
}


void optimize_start(cv::Mat &image, int psize)
{
	cv::Mat nnf, last_nnf;
	cv::Mat dst = image.clone();

	while (contains_hole(image))
	{
		initialize_hole_NNF(nnf, image, dst, psize, last_nnf);
		iterate_hole_NNF(nnf, psize, image, dst);
		reconstruct_from_hole_NNF_no_avg(nnf, image, dst, psize);
		last_nnf.release();
		last_nnf = nnf.clone();
	}

	initialize_hole_NNF(nnf, image, dst, psize, last_nnf);
	iterate_hole_NNF(nnf, psize, image, dst);
	reconstruct_from_hole_NNF(nnf, image, dst, 3);

	dst.release();
	nnf.release();
	last_nnf.release();
}
void ImgFilter2d( cv::Mat &image)
 {
	 cv::Mat result;
	    result.create(image.size(), image.type());
	     int nr = image.rows;
	     int nc = image.cols*image.channels();
	     for (int i = 1; i < nr - 1; i++)
		     {
		         const uchar* up_line = image.ptr<uchar>(i - 1);//指向上一行
		         const uchar* mid_line = image.ptr<uchar>(i);//当前行
	        const uchar* down_line = image.ptr<uchar>(i + 1);//下一行
		         uchar* cur_line = result.ptr<uchar>(i);
		         for (int j = 1; j < nc - 1; j++)
			         {
			             cur_line[j] = cv::saturate_cast<uchar>(5 * mid_line[j] - mid_line[j - 1] - mid_line[j + 1] -
				                 up_line[j] - down_line[j]);
			         }
	     }
	     // 把图像边缘像素设置为0
		    result.row(0).setTo(cv::Scalar(0));
	    result.row(result.rows - 1).setTo(cv::Scalar(0));
	    result.col(0).setTo(cv::Scalar(0));
	     result.col(result.cols - 1).setTo(cv::Scalar(0));
		 result.assignTo(image);
		 result.release();
 }
void colorReduce0(cv::Mat &image, int div = 64) {
	int nr = image.rows; // number of rows  
	int nc = image.cols * image.channels(); // total number of elements per line  
	for (int j = 0; j < nr; j++) {
		uchar* data = image.ptr<uchar>(j);
		for (int i = 0; i < nc; i++) {
			data[i] = data[i] / div*div + div / 2;
		}
	}
}
void SSR(IplImage* src, int sigma, int scale)
{
	IplImage* src_fl = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, src->nChannels);
	IplImage* src_fl1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, src->nChannels);
	IplImage* src_fl2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, src->nChannels);
	float a = 0.0, b = 0.0, c = 0.0;
	cvConvertScale(src, src_fl, 1.0, 1.0);//转换范围，所有图像元素增加1.0保证cvlog正常
	cvLog(src_fl, src_fl1);

	cvSmooth(src_fl, src_fl2, CV_GAUSSIAN, 0, 0, sigma);        //SSR算法的核心之一，高斯模糊

	cvLog(src_fl2, src_fl2);
	cvSub(src_fl1, src_fl2, src_fl);//Retinex公式，Log(R(x,y))=Log(I(x,y))-Log(Gauss(I(x,y)))

	//计算图像的均值、方差，SSR算法的核心之二
	//使用GIMP中转换方法：使用图像的均值方差等信息进行变换
	//没有添加溢出判断
	CvScalar mean;
	CvScalar dev;
	cvAvgSdv(src_fl, &mean, &dev, NULL);//计算图像的均值和标准差
	double min[3];
	double max[3];
	double maxmin[3];
	for (int i = 0; i < 3; i++)
	{
		min[i] = mean.val[i] - scale*dev.val[i];
		max[i] = mean.val[i] + scale*dev.val[i];
		maxmin[i] = max[i] - min[i];
	}
	float* data2 = (float*)src_fl->imageData;
	for (int i = 0; i < src_fl2->width; i++)
	{
		for (int j = 0; j < src_fl2->height; j++)
		{
			data2[j*src_fl->widthStep / 4 + 3 * i + 0] = 255 * (data2[j*src_fl->widthStep / 4 + 3 * i + 0] - min[0]) / maxmin[0];
			data2[j*src_fl->widthStep / 4 + 3 * i + 1] = 255 * (data2[j*src_fl->widthStep / 4 + 3 * i + 1] - min[1]) / maxmin[1];
			data2[j*src_fl->widthStep / 4 + 3 * i + 2] = 255 * (data2[j*src_fl->widthStep / 4 + 3 * i + 2] - min[2]) / maxmin[2];
		}
	}


	cvConvertScale(src_fl, src, 1, 0);
	cvReleaseImage(&src_fl);
	cvReleaseImage(&src_fl1);
	cvReleaseImage(&src_fl2);
}
void test()
{
	char* filename = "E:\\input_0.png";
	IplImage* frog = cvLoadImage(filename, 1);
	IplImage* frog1 = cvCreateImage(cvGetSize(frog), IPL_DEPTH_32F, frog->nChannels);
	cvConvertScale(frog, frog1, 1.0 / 255, 0);
	//SSR(frog, 30, 10);
	Retinex(frog, 200, 128, 128);
	cvNamedWindow("hi");
	cvNamedWindow("hi1");
	cvShowImage("hi1", frog1);
	cvShowImage("hi", frog);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&frog);
	cvReleaseImage(&frog1);
}
#include "FindEdges.h"
#include "BoxBlur.h"
#include "Otsu.h"
#include "HistgramBinaryzation.h"
#include "TVDenoising.h"
#include "Canny.h"
#include "MoravecFP.h"
#include "Guildedfilter.h"
#include "GlobalFun.h"
#include "cprgb2gray.h"
#include "sgss/lens_blur_filter.h"
void COpenCVPlatView::OnHistogram()
{
	COpenCVPlatDoc* pDoc = GetDocument();
	cv::Mat &image = pDoc->image;
	cv::Mat imageclone = image.clone();
	//cprgb2gray::declor(image, imageclone, 0.2);
//	GlobalFun::Devignetting_Original(imageclone.data, image.data, image.cols, image.rows);
//	BoxBlur::Process(image, 5, Tile);
	//FindEdges::SalientRegionDetectionBasedonLC(image);
//	HistgramBinaryzation::HuangFuzzyThreshold(image);
	//TVDenoising::ProcessTVD(image, 80);
/*	Guildedfilter* pGuidfilter = new Guildedfilter();
	cv::Mat imagep;
	cvtColor(image, imagep, CV_BGR2GRAY);	
	cv::Mat guideImg;
	imagep.convertTo(guideImg, CV_64FC1, 1 / 255.0f);
	cv::Mat inputimg;
	imagep.convertTo(inputimg, CV_64FC1, 1 / 255.0f);
	int  nc = inputimg.channels();
	cv::Mat result = pGuidfilter->GuidedFilterMat(guideImg, inputimg, 4, 0.006);*/
	
	//result.convertTo(image, CV_8UC1,255.0);
//	cv::imshow("qq3", image);
//	Canny::CannyProcess(image);
	//bilateralFilter::Process(image);
//	colorReduce0(image);
//	test();
//	ImgFilter2d(image);
	/*CHistogramCalc* pHist = new CHistogramCalc();
	pHist->calculateHistogram(image);
	int32_t red[256] = { 0 };
	int32_t green[256] = { 0 };
	int32_t blue[256] = { 0 };
//	pHist->Histogram(image, red, green, blue, NULL, 0);
	//pHist->InteEqualize(image);
	pHist->AHE(image, 4);
	delete pHist;*/
/*	int i;
//	double r_vec[3] = { -2.100418, -2.167796, 0.273330 };
	double r_vec[3] = { 0};
	double R_matrix[9] = { -0.036255,
		0.978364,
		- 0.203692,
		0.998304,
		0.026168,
		- 0.051995,
		- 0.045539,
		- 0.205232,
		- 0.977653 };
	CvMat pr_vec;
	CvMat pR_matrix;

	cvInitMatHeader(&pr_vec, 1, 3, CV_64FC1, r_vec, CV_AUTOSTEP);
	cvInitMatHeader(&pR_matrix, 3, 3, CV_64FC1, R_matrix, CV_AUTOSTEP);
	cvRodrigues2(&pR_matrix, &pr_vec, 0);
	
	for (i = 0; i < 9; i++)
	{
		printf("%f\n", R_matrix[i]);
	}*/
	const cv::Mat source(cv::imread("I:/GitHub/ImageTest/OpenCVPlat/data/image.jpg"));
	const cv::Mat mask(cv::imread("I:/GitHub/ImageTest/OpenCVPlat/data/linear.jpg", cv::IMREAD_GRAYSCALE));
	const cv::Mat kernel(cv::imread("I:/GitHub/ImageTest/OpenCVPlat/data/radial.jpg", cv::IMREAD_GRAYSCALE));
	cv::Mat destination;
	sgss::LensBlurFilter filter(kernel, cv::Size(27, 27));
	filter.set_gradient(mask);
	filter.set_brightness(6.0);
	filter(source, &destination);
	cv::imshow("", destination);
	cv::waitKey();
	Invalidate();
	// TODO: Add your command handler code here
}
