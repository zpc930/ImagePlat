#include "stdafx.h"
#include "Canny.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
Canny::Canny()
{
}


Canny::~Canny()
{
}
int Canny::GaussBlur(cv::Mat& grayImage, double nSigma)
{
	
	int nWindowSize = 1 + 2 * ceil(3 * nSigma);//通过sigma得到窗口大小
	int nCenter = nWindowSize / 2;
	int nWidth = grayImage.cols;
	int nHeight = grayImage.rows;
	cv::Mat grayblurImage = grayImage.clone();
	double *pKernel_2 = new double[nWindowSize*nWindowSize];
	double d_sum = 0.0;
	for (int y = 0; y < nWindowSize; y++)
	{
		for (int x = 0; x < nWindowSize; x++)
		{
			double n_Disx = y - nCenter;//水平方向距离中心像素距离
			double n_Disy = x - nCenter;
			pKernel_2[x*nWindowSize + y] = exp(-0.5*(n_Disx*n_Disx + n_Disy*n_Disy) / (nSigma*nSigma)) / (2.0*3.1415926)*nSigma*nSigma;
			d_sum = d_sum + pKernel_2[x*nWindowSize + y];
		}
	}
	for (int i = 0; i < nWindowSize; i++)//归一化处理
	{
		for (int j = 0; j < nWindowSize; j++)
		{
			pKernel_2[j*nWindowSize + i] = pKernel_2[j*nWindowSize + i] / d_sum;
		}
	}
	for (int s = 0; s < nHeight; s++)
	{
		for (int t = 0; t < nWidth; t++)
		{
			double dFilter = 0;
			double dSum = 0;
			//当前坐标（s,t）
			//获取8邻域
			for (int i = -nCenter; i < nCenter; i++)
			{
				for (int j = 0 - nCenter; j < nCenter;j++)
				{
					if ((s + i) >= 0 && (s + i)<nHeight && (t + j) >= 0 && (t+j)<nWidth)
					{
						uchar *data = grayImage.ptr<uchar>(s + i);
						double currentvalue =(double) data[t + j];
						dFilter += currentvalue*pKernel_2[j + nCenter + (i + nCenter)*nCenter];
						dSum += pKernel_2[j + nCenter + (i + nCenter)*nCenter];
					}
				}
			}
			uchar *blurdata = grayblurImage.ptr<uchar>(s);
			blurdata[t] = (uchar)(dFilter / dSum);
		}
	}
	grayImage = grayblurImage;
	return 0;
}

//其中，邻域跟踪算法给出了两个，一种是递归，一种是非递归。

//递归算法。解决了堆栈溢出问题。之前找了很多canny代码参考，其中有一个版本流传很广，但是对于大图像堆栈溢出。

/*int TraceEdge(int w, int h, int nThrLow, cv::Mat& f_image, int *pMag)
 {
	    int n, m;
	     char flag = 0;
		 uchar *data = f_image.ptr<uchar>(h);
	     int currentvalue = (uchar)data[w];
		 int nwidth = f_image.cols;
	     if (currentvalue == 0)
		    {
				data[w] = 255;
		        flag = 0;
		         for (n = -1; n <= 1; n++)
			         {
			             for (m = -1; m <= 1; m++)
				             {
				                 if (n == 0 && m == 0) continue;
								 data = f_image.ptr<uchar>(h+m);
								 int curgrayvalue = (uchar)data[w+n]; 
								 int curgrdvalue = pMag[w + n + (h + m)*nwidth];
				                 if (curgrayvalue == 200 && curgrdvalue > nThrLow)
									 if (TraceEdge(w + n, h + m, nThrLow, f_image, pMag))
					                     {
					                         flag = 1;
					                        break;
					                     }
				             }
			             if (flag) break;
			         }
		         return(1);
		     }
	    return(0);
	 }*/
void TraceEdge(int w, int h, int nThrLow, cv::Mat& f_image, int *pMag)
{
	     //对8邻域像素进行查询  
		    int xNum[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	     int yNum[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	     int xx = 0;
	     int yy = 0;
	     bool change = true;
		 int nwidth = f_image.cols;
	     while (change)
		 {
		     change = false;
			for (int k = 0; k < 8; k++)
			{
			xx = w + xNum[k];
			yy = h + yNum[k];
			//   如果该象素为可能的边界点，又没有处理过  
			//   并且梯度大于阈值  
			uchar *data = f_image.ptr<uchar>(yy);
			int curgrayvalue = (uchar)data[xx];
			int curgrdvalue = pMag[xx + yy*nwidth];
			if (curgrayvalue == 200 && curgrdvalue > nThrLow)
			{
				change = true;
				//   把该点设置成为边界点  
				data[xx] = 255;
				h = yy;
				 w = xx;
				break;
				}
			}
	}
}

int Canny::CannyProcess(cv::Mat& f_image)
{
	cv::Mat gray;
	cv::cvtColor(f_image, gray, CV_BGR2GRAY);
	GaussBlur(gray, 0.1);
	//cv::namedWindow("GaussImage");

	//// Display the image m in this window
	//cv::imshow("GaussImage", gray);               //显示高斯图
	//cvWaitKey(0);
	//cv::destroyWindow("GaussImage");

	//第三步：计算梯度值和方向 
	int nWidth = gray.cols;
	int nHeight = gray.rows;
	double *P = new double[nWidth*nHeight];
	double *Q = new double[nHeight*nWidth];
	int* M = new int[nHeight*nWidth];
	memset(M, 0, sizeof(int)*nHeight*nWidth);
	double *Theta = new double[nHeight*nWidth];
	for (int iH = 0; iH < nHeight - 1; iH++)
	{
		for (int iW = 0; iW < nWidth - 1; iW++)
		{
			uchar *data = gray.ptr<uchar>(iH);
			double d1 = data[min(iW + 1, nWidth - 1)];
			double d2 = data[iW];
			data = gray.ptr<uchar>(min(iH + 1, nHeight - 1));
			double d3 = data[min(iW + 1, nWidth - 1)];
			double d4 = data[iW];
			P[iH*nWidth + iW] = (d1 - d2 + d3 - d4) / 2;
			data = gray.ptr<uchar>(iH);
			d1 = data[iW];
			d2 = data[min(iW + 1, nWidth - 1)];
			data = gray.ptr<uchar>(min(iH + 1, nHeight - 1));
			d3 = data[iW];
			d4 = data[min(iW + 1, nWidth - 1)];
			Q[iH*nWidth + iW] = (d1 - d3 + d4 - d2) / 2;
		}
	}
	//计算幅值和方向
	for (int y = 0; y < nHeight - 1; y++)
	{
		for (int x = 0; x < nWidth - 1; x++)
		{
			M[y*nWidth + x] = (int)sqrt(P[y*nWidth + x] * P[y*nWidth + x] + Q[y*nWidth + x] * Q[y*nWidth + x] + 0.5);
			Theta[y*nWidth + x] = atan2(Q[y*nWidth + x], P[y*nWidth + x]) * 180 / 3.1415;
			if (Theta[y*nWidth + x] < 0)
			{
				Theta[y*nWidth + x] = 360 + Theta[y*nWidth + x];
			}
		}
	}
	////////第四步：非极大值抑制
	cv::Mat N = gray.clone();//非极大值抑制结果
	cv::Mat OpencvCannyimg = gray.clone();
	int g1 = 0, g2 = 0, g3 = 0, g4 = 0;                            //用于进行插值，得到亚像素点坐标值   
	double dTmp1 = 0.0, dTmp2 = 0.0;                           //保存两个亚像素点插值得到的灰度数据 
	double dWeight = 0.0;                                    //插值的权重  

	for (int i = 1; i < nHeight - 1; i++)
	{
		for (int j = 1; j < nWidth - 1; j++)
		{
			//如果当前点梯度为0，该点就不是边缘点
			if (M[j + i*nWidth] == 0)
			{
				uchar *data = OpencvCannyimg.ptr<uchar>(i);
				data[j] = 0;
			}
			else
			{
				////////首先判断属于那种情况，然后根据情况插值///////  
				////////////////////第一种情况///////////////////////  
				/////////       g1  g2                  /////////////  
				/////////           C                   /////////////  
				/////////           g3  g4              /////////////  
				///////////////////////////////////////////////////// 
				if ((Theta[j + i*nWidth] >= 90 && Theta[j + i*nWidth] < 135) || (Theta[j + i*nWidth] >= 270 && Theta[j + i*nWidth] < 315))
				{
					g1 = M[j - 1 + (i - 1)*nWidth];
					g2 = M[j + (i - 1)*nWidth];
					g3 = M[j + (i + 1)*nWidth];
					g4 = M[j + 1 + (i + 1)*nWidth];
					dWeight = fabs(P[j + i*nWidth]) / fabs(Q[j + i*nWidth]);
					dTmp1 = g1*dWeight + (1 - dWeight)*g2;
					dTmp2 = g4*dWeight + (1 - dWeight)*g3;
				}
				////////////////////第二种情况///////////////////////  
				/////////       g1                      /////////////  
				/////////       g2  C   g3              /////////////  
				/////////               g4              /////////////  
				/////////////////////////////////////////////////////  
				else if ((Theta[j + i*nWidth] >= 135 && Theta[j + i*nWidth] < 180) || (Theta[j + i*nWidth] >= 315 && Theta[j + i*nWidth] < 360))
				{
					g1 = M[j - 1 + (i - 1)*nWidth];
					g2 = M[j - 1 + (i)*nWidth];
					g3 = M[j + 1 + (i)*nWidth];
					g4 = M[j + 1 + (i + 1)*nWidth];
					dWeight = fabs(Q[j + i*nWidth]) / fabs(P[j + i*nWidth]);
					dTmp1 = g1*dWeight + (1 - dWeight)*g2;
					dTmp2 = g4*dWeight + (1 - dWeight)*g3;
				}
				////////////////////第三种情况///////////////////////  
				/////////           g1  g2              /////////////  
				/////////           C                   /////////////  
				/////////       g4  g3                  /////////////  
				///////////////////////////////////////////////////// 
				else if ((Theta[j + i*nWidth] >= 45 & Theta[j + i*nWidth] < 90) || (Theta[j + i*nWidth] >= 225 & Theta[j + i*nWidth] < 270))
				{
					g1 = M[j + 1 + (i - 1)*nWidth];
					g2 = M[j + (i - 1)*nWidth];
					g3 = M[j + (i + 1)*nWidth];
					g4 = M[j - 1 + (i + 1)*nWidth];
					dWeight = fabs(P[j + i*nWidth]) / fabs(Q[j + i*nWidth]);
					dTmp1 = g1*dWeight + (1 - dWeight)*g2;
					dTmp2 = g4*dWeight + (1 - dWeight)*g3;
				}
				////////////////////第四种情况///////////////////////  
				/////////               g1              /////////////  
				/////////       g4  C   g2              /////////////  
				/////////       g3                      /////////////  
				///////////////////////////////////////////////////// 
				else if ((Theta[j + i*nWidth] >= 0 & Theta[j + i*nWidth] < 45) || (Theta[j + i*nWidth] >= 180 & Theta[j + i*nWidth] < 225))
				{
					g1 = M[j + 1 + (i - 1)*nWidth];
					g2 = M[j + 1 + (i)*nWidth];
					g3 = M[j - 1 + (i)*nWidth];
					g4 = M[j - 1 + (i + 1)*nWidth];
					dWeight = fabs(Q[j + i*nWidth]) / fabs(P[j + i*nWidth]);
					dTmp1 = g1*dWeight + (1 - dWeight)*g2;
					dTmp2 = g4*dWeight + (1 - dWeight)*g3;
				}
			}
			if ((M[j + i*nWidth] >= dTmp1) && (M[j + i*nWidth] >= dTmp2))
			{
				uchar *data = OpencvCannyimg.ptr<uchar>(i);
				data[j] = 200;
			}
			else
			{
				uchar *data = OpencvCannyimg.ptr<uchar>(i);
				data[j] = 0;
			}


		}
	}
	//cv::namedWindow("Limteimg");

	//// Display the image m in this window
	//cv::imshow("Limteimg", OpencvCannyimg);             
	//cvWaitKey(0);
	//cv::destroyWindow("Limteimg");
	///////第五步：双阈值的选取
	//注意事项  注意是梯度幅值的阈值 
	int nHist[1024];//直方图
	int nEdgeNum;//所有边缘点的数目
	int nMaxMag = 0;//最大梯度的幅值
	for (int k = 0; k < 1024; k++)
	{
		nHist[k] = 0;
	}
	for (int hy = 0; hy < nHeight; hy++)
	{
		for (int wx = 0; wx < nWidth; wx++)
		{
			uchar *data = OpencvCannyimg.ptr<uchar>(hy);
			if ((uchar)data[wx] == 200)
			{
				int Mindex = M[wx + hy*nWidth];
				nHist[M[wx + hy*nWidth]]++;//获取了梯度直方图
			}
		}
	}
	nEdgeNum = 0;
	for (int index = 1; index < 1024; index++)
	{
		if (nHist[index] != 0)
		 {
			 nMaxMag = index;
		}
		nEdgeNum += nHist[index];//经过non-maximum suppression后有多少边缘点像素  
	}
	//计算两个阈值 注意是梯度的阈值
	int nThrHigh;
	int nThrLow;
	double dRateHigh = 0.7;
	double dRateLow = 0.5;
	int nHightcount = (int)(dRateHigh*nEdgeNum + 0.5);
	 int count = 1;
	nEdgeNum = nHist[1];
	while ((nEdgeNum <= nHightcount) && (count < nMaxMag - 1))
	{
	 count++;
	 nEdgeNum += nHist[count];
	 }
	nThrHigh = count;
	nThrLow = (int)(nThrHigh*dRateLow + 0.5);
	for (int is = 1; is < nHeight - 1; is++)
	 {
		for (int jt = 1; jt < nWidth - 1; jt++)
		{
			//CvScalar s=cvGet2D(N,jt,is);
			//int currentvalue=s.val[0];
			uchar *data = OpencvCannyimg.ptr<uchar>(is);
			int currentvalue = (uchar)(data[jt]);
			 if ((currentvalue == 200) && (M[jt + is*nWidth] >= nThrHigh))
				//是非最大抑制后的点且 梯度幅值要大于高阈值
				{
					data[jt] = 255;
				    //邻域点判断
					TraceEdge(jt, is, nThrLow, OpencvCannyimg, M);
				 }
			}
		}
	 for (int si = 1; si < nHeight - 1; si++)
		{
		 for (int tj = 1; tj < nWidth - 1; tj++)
		 {
			 uchar *data = OpencvCannyimg.ptr<uchar>(si);
			 if ((uchar)data[ tj] != 255)
			 {
				 data[tj] = 0;
			}
		}

	}
	 f_image = OpencvCannyimg;
	return 0;
}