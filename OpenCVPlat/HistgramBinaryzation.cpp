#include "stdafx.h"
#include "HistgramBinaryzation.h"


HistgramBinaryzation::HistgramBinaryzation()
{
}


HistgramBinaryzation::~HistgramBinaryzation()
{
}
//灰度平局值值法：
int HistgramBinaryzation::GetMeanThreshold(int* HistGram, int nSzie)
{
	int Sum = 0, Amount = 0;
	for (int Y = 0; Y < nSzie; Y++)
	{
		Amount += HistGram[Y];
		Sum += Y * HistGram[Y];
	}
	return Sum / Amount;
}
//百分比阈值（P-Tile法）
 int HistgramBinaryzation::GetPTileThreshold(int* HistGram, int Tile , int nSize)
{
	int Y, Amount = 0, Sum = 0;
	for (Y = 0; Y < nSize; Y++) Amount += HistGram[Y];        //  像素总数
	for (Y = 0; Y < nSize; Y++)
	{
		Sum = Sum + HistGram[Y];
		if (Sum >= Amount * Tile / 100) return Y;
	}
	return -1;
}
 /*三、基于谷底最小值的阈值

	 1、描述：

	 此方法实用于具有明显双峰直方图的图像，其寻找双峰的谷底作为阈值，但是该方法不一定能获得阈值，对于那些具有平坦的直方图或单峰图像，该方法不合适。

	 2、实现过程：

	 该函数的实现是一个迭代的过程，每次处理前对直方图数据进行判断，看其是否已经是一个双峰的直方图，如果不是，则对直方图数据进行半径为1（窗口大小为3）的平滑，如果迭代了一定的数量比如1000次后仍未获得一个双峰的直方图，则函数执行失败，如成功获得，则最终阈值取两个双峰之间的谷底值作为阈值。

	 注意在编码过程中，平滑的处理需要当前像素之前的信息，因此需要对平滑前的数据进行一个备份。另外，首数据类型精度限制，不应用整形的直方图数据，必须转换为浮点类型数据来进行处理，否则得不到正确的结果。
 */
  static bool IsDimodal(double* HistGram,int nSize)       // 检测直方图是否为双峰的
 {
	 // 对直方图的峰进行计数，只有峰数位2才为双峰 
	 int Count = 0;
	 for (int Y = 1; Y < nSize-1; Y++)
	 {
		 if (HistGram[Y - 1] < HistGram[Y] && HistGram[Y + 1] < HistGram[Y])
		 {
			 Count++;
			 if (Count > 2) return false;
		 }
	 }
	 if (Count == 2)
		 return true;
	 else
		 return false;
 }
  static int GetMinimumThreshold(int* HistGram,int nSize)
 {
	 int Y, Iter = 0;
	 double* HistGramC = new double[nSize];           // 基于精度问题，一定要用浮点数来处理，否则得不到正确的结果
	 double* HistGramCC = new double[nSize];          // 求均值的过程会破坏前面的数据，因此需要两份数据
	 for (Y = 0; Y < nSize; Y++)
	 {
		 HistGramC[Y] = HistGram[Y];
		 HistGramCC[Y] = HistGram[Y];
	 }

	 // 通过三点求均值来平滑直方图
	 while (IsDimodal(HistGramCC,nSize) == false)                                        // 判断是否已经是双峰的图像了      
	 {
		 HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;                 // 第一点
		 for (Y = 1; Y < nSize; Y++)
			 HistGramCC[Y] = (HistGramC[Y - 1] + HistGramC[Y] + HistGramC[Y + 1]) / 3;     // 中间的点
		 HistGramCC[nSize - 1] = (HistGramC[nSize - 2] + HistGramC[nSize - 1] + HistGramC[nSize-1]) / 3;         // 最后一点
		 memcpy(HistGramCC, HistGramC, nSize * sizeof(double));
		 Iter++;
		 if (Iter >= 1000) return -1;                                                   // 直方图无法平滑为双峰的，返回错误代码
	 }
	 // 阈值极为两峰之间的最小值 
	 bool Peakfound = false;
	 for (Y = 1; Y < nSize-1; Y++)
	 {
		 if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peakfound = true;
		 if (Peakfound == true && HistGramCC[Y - 1] >= HistGramCC[Y] && HistGramCC[Y + 1] >= HistGramCC[Y])
			 return Y - 1;
	 }
	 return -1;
 }
  //四、基于双峰平均值的阈值
//	  1、描述：
//  该算法和基于谷底最小值的阈值方法类似，只是最后一步不是取得双峰之间的谷底值，而是取双峰的平均值作为阈值。
  static int GetIntermodesThreshold(int* HistGram,int nSize)
  {
	  int Y, Iter = 0, Index;
	  double* HistGramC = new double[nSize];           // 基于精度问题，一定要用浮点数来处理，否则得不到正确的结果
	  double* HistGramCC = new double[nSize];          // 求均值的过程会破坏前面的数据，因此需要两份数据
	  for (Y = 0; Y < nSize; Y++)
	  {
		  HistGramC[Y] = HistGram[Y];
		  HistGramCC[Y] = HistGram[Y];
	  }
	  // 通过三点求均值来平滑直方图
	  while (IsDimodal(HistGramCC,nSize) == false)                                                  // 判断是否已经是双峰的图像了      
	  {
		  HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;                   // 第一点
		  for (Y = 1; Y < nSize-1; Y++)
			  HistGramCC[Y] = (HistGramC[Y - 1] + HistGramC[Y] + HistGramC[Y + 1]) / 3;       // 中间的点
		  HistGramCC[nSize - 1] = (HistGramC[nSize - 2] + HistGramC[nSize - 1] + HistGramC[nSize-1]) / 3;           // 最后一点
		  memcpy(HistGramCC, HistGramC, nSize * sizeof(double));         // 备份数据，为下一次迭代做准备
		  Iter++;
		  if (Iter >= 10000) return -1;                                                       // 似乎直方图无法平滑为双峰的，返回错误代码
	  }
	  // 阈值为两峰值的平均值
	  int* Peak = new int[2];
	  for (Y = 1, Index = 0; Y < nSize-1; Y++)
	  if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peak[Index++] = Y - 1;
	  int Threshold = ((Peak[0] + Peak[1]) / 2);
	  delete[] Peak;
	  delete[] HistGramC;
	  delete[] HistGramCC;
	  return Threshold;
  }
 /* 五、迭代最佳阈值

	  1、描述：

	  该算法先假定一个阈值，然后计算在该阈值下的前景和背景的中心值，当前景和背景中心值得平均值和假定的阈值相同时，则迭代中止，并以此值为阈值进行二值化。

	  2、实现过程：

	  （1）求出图象的最大灰度值和最小灰度值，分别记为gl和gu，令初始阈值为：



	  (2) 根据阈值T0将图象分割为前景和背景，分别求出两者的平均灰度值Ab和Af:



  (3) 令



	  如果Tk = Tk + 1, 则取Tk为所求得的阈值，否则，转2继续迭代。*/
   int GetIterativeBestThreshold(int* HistGram,int nSize)
  {
	  int X, Iter = 0;
	  int MeanValueOne, MeanValueTwo, SumOne, SumTwo, SumIntegralOne, SumIntegralTwo;
	  int MinValue, MaxValue;
	  int Threshold, NewThreshold;

	  for (MinValue = 0; MinValue < nSize && HistGram[MinValue] == 0; MinValue++);
	  for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);

	  if (MaxValue == MinValue) return MaxValue;          // 图像中只有一个颜色             
	  if (MinValue + 1 == MaxValue) return MinValue;      // 图像中只有二个颜色

	  Threshold = MinValue;
	  NewThreshold = (MaxValue + MinValue) >> 1;
	  while (Threshold != NewThreshold)    // 当前后两次迭代的获得阈值相同时，结束迭代    
	  {
		  SumOne = 0; SumIntegralOne = 0;
		  SumTwo = 0; SumIntegralTwo = 0;
		  Threshold = NewThreshold;
		  for (X = MinValue; X <= Threshold; X++)         //根据阈值将图像分割成目标和背景两部分，求出两部分的平均灰度值      
		  {
			  SumIntegralOne += HistGram[X] * X;
			  SumOne += HistGram[X];
		  }
		  MeanValueOne = SumIntegralOne / SumOne;
		  for (X = Threshold + 1; X <= MaxValue; X++)
		  {
			  SumIntegralTwo += HistGram[X] * X;
			  SumTwo += HistGram[X];
		  }
		  MeanValueTwo = SumIntegralTwo / SumTwo;
		  NewThreshold = (MeanValueOne + MeanValueTwo) >> 1;       //求出新的阈值
		  Iter++;
		  if (Iter >= 1000) return -1;
	  }
	  return Threshold;
  }
   /*六、OSTU大律法

　　1、描述：

  该算法是1979年由日本大津提出的，主要是思想是取某个阈值，使得前景和背景两类的类间方差最大，matlab中的graythresh即是以该算法为原理执行的。

  2、原理：

  关于该算法的原理，网络上有很多，这里为了篇幅有限，不加以赘述。

  3、参考代码：*/
    int GetOSTUThreshold(int* HistGram,int nSize)
   {
	   int  Y, Amount = 0;
	   int PixelBack = 0, PixelFore = 0, PixelIntegralBack = 0, PixelIntegralFore = 0, PixelIntegral = 0;
	   double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;              // 类间方差;
	   int MinValue, MaxValue;
	   int Threshold = 0;

	   for (MinValue = 0; MinValue < nSize && HistGram[MinValue] == 0; MinValue++);
	   for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);
	   if (MaxValue == MinValue) return MaxValue;          // 图像中只有一个颜色             
	   if (MinValue + 1 == MaxValue) return MinValue;      // 图像中只有二个颜色

	   for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  像素总数

	   PixelIntegral = 0;
	   for (Y = MinValue; Y <= MaxValue; Y++) PixelIntegral += HistGram[Y] * Y;
	   SigmaB = -1;
	   for (Y = MinValue; Y < MaxValue; Y++)
	   {
		   PixelBack = PixelBack + HistGram[Y];
		   PixelFore = Amount - PixelBack;
		   OmegaBack = (double)PixelBack / Amount;
		   OmegaFore = (double)PixelFore / Amount;
		   PixelIntegralBack += HistGram[Y] * Y;
		   PixelIntegralFore = PixelIntegral - PixelIntegralBack;
		   MicroBack = (double)PixelIntegralBack / PixelBack;
		   MicroFore = (double)PixelIntegralFore / PixelFore;
		   Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);
		   if (Sigma > SigmaB)
		   {
			   SigmaB = Sigma;
			   Threshold = Y;
		   }
	   }
	   return Threshold;
   }
	/* 七、一维最大熵

	  1、描述：

	  该算法把信息论中熵的概念引入到图像中，通过计算阈值分割后两部分熵的和来判断阈值是否为最佳阈值。

	  2、算法原理

	  这方面的文章也比较多，留给读者自行去查找相关资料。*/
	int Get1DMaxEntropyThreshold(int* HistGram)
	{
		int  X, Y, Amount = 0;
		double* HistGramD = new double[256];
		double SumIntegral, EntropyBack, EntropyFore, MaxEntropy;
		int MinValue = 255, MaxValue = 0;
		int Threshold = 0;

		for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);
		for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);
		if (MaxValue == MinValue) return MaxValue;          // 图像中只有一个颜色             
		if (MinValue + 1 == MaxValue) return MinValue;      // 图像中只有二个颜色

		for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  像素总数

		for (Y = MinValue; Y <= MaxValue; Y++)   HistGramD[Y] = (double)HistGram[Y] / Amount + 1e-17;

		MaxEntropy = -1.7e+308;
		for (Y = MinValue + 1; Y < MaxValue; Y++)
		{
			SumIntegral = 0;
			for (X = MinValue; X <= Y; X++) SumIntegral += HistGramD[X];
			EntropyBack = 0;
			for (X = MinValue; X <= Y; X++) EntropyBack += (-HistGramD[X] / SumIntegral * log(HistGramD[X] / SumIntegral));
			EntropyFore = 0;
			for (X = Y + 1; X <= MaxValue; X++) EntropyFore += (-HistGramD[X] / (1 - SumIntegral) * log(HistGramD[X] / (1 - SumIntegral)));
			if (MaxEntropy < EntropyBack + EntropyFore)
			{
				Threshold = Y;
				MaxEntropy = EntropyBack + EntropyFore;
			}
		}
		return Threshold;
	}
	/*八、力矩保持法
		1、描述:

	该算法通过选择恰当的阈值从而使得二值后的图像和原始的灰度图像具有三个相同的初始力矩值。

		2、原理：

		参考论文：W.Tsai, “Moment - preserving thresholding : a new approach, ” Comput.Vision Graphics Image Process., vol. 29, pp. 377 - 393, 1985.

		由于无法下载到该论文（收费的），仅仅给出从其他一些资料中找到的公式共享一下。



		其中的A\B\C的函数可见代码部分。*/
	 double A(int* HistGram, int Index)
	{
		double Sum = 0;
		for (int Y = 0; Y <= Index; Y++)
			Sum += HistGram[Y];
		return Sum;
	}

	 double B(int* HistGram, int Index)
	{
		double Sum = 0;
		for (int Y = 0; Y <= Index; Y++)
			Sum += (double)Y * HistGram[Y];
		return Sum;
	}

	double C(int* HistGram, int Index)
	{
		double Sum = 0;
		for (int Y = 0; Y <= Index; Y++)
			Sum += (double)Y * Y * HistGram[Y];
		return Sum;
	}

	static double D(int* HistGram, int Index)
	{
		double Sum = 0;
		for (int Y = 0; Y <= Index; Y++)
			Sum += (double)Y * Y * Y * HistGram[Y];
		return Sum;
	}
	 byte GetMomentPreservingThreshold(int* HistGram)
	{
		int  Y, Index = 0, Amount = 0;
		double* Avec = new double[256];
		double X2, X1, X0, Min;

		for (Y = 0; Y <= 255; Y++) Amount += HistGram[Y];        //  像素总数
		for (Y = 0; Y < 256; Y++) Avec[Y] = (double)A(HistGram, Y) / Amount;       // The threshold is chosen such that A(y,t)/A(y,n) is closest to x0.

		// The following finds x0.

		X2 = (double)(B(HistGram, 255) * C(HistGram, 255) - A(HistGram, 255) * D(HistGram, 255)) / (double)(A(HistGram, 255) * C(HistGram, 255) - B(HistGram, 255) * B(HistGram, 255));
		X1 = (double)(B(HistGram, 255) * D(HistGram, 255) - C(HistGram, 255) * C(HistGram, 255)) / (double)(A(HistGram, 255) * C(HistGram, 255) - B(HistGram, 255) * B(HistGram, 255));
		X0 = 0.5 - (B(HistGram, 255) / A(HistGram, 255) + X2 / 2) / sqrt(X2 * X2 - 4 * X1);

		for (Y = 0, Min = -1.7e+308; Y < 256; Y++)
		{
			if (abs(Avec[Y] - X0) < Min)
			{
				Min = abs(Avec[Y] - X0);
				Index = Y;
			}
		}
		return (byte)Index;
	}

	 int HistgramBinaryzation::WellneradaptiveThreshold1(cv::Mat &image, int Radius, int Threshold)
	 {
		

		 if (Radius < 0 || Radius > 255) return 2;
		 if (Threshold < 0 || Threshold > 100) return 0;
		 int Width, Height,  X, Y;
		 int Sum, InvertThreshold, XX ;
		 byte* Pointer;
		 Width = image.cols;
		 Height = image.rows; 
		 cv::Mat imagegray;
		 imagegray.create(Height, Width, CV_8UC1);
		 cvtColor(image, imagegray, CV_BGR2GRAY);

		 InvertThreshold = 100 - Threshold;
		 byte* Row = (byte*)malloc(Width); 
		 for (Y = 0; Y < Height; Y++)
		 {
			 Pointer = imagegray.ptr<uchar>(Y);
			 Sum = *Pointer * Radius;
			 memcpy(Row, Pointer, Width);
			 for (X = 0; X < Width; X++)
			 {
				 XX = X - Radius;
				 if (XX < 0) XX = 0;
				 Sum += Row[X] - Row[XX];
				 if (Row[X] * 100 * Radius < Sum * InvertThreshold)
					 Pointer[X] = 0;
				 else
					 Pointer[X] = 255;
			 }
		 }
		 free(Row);
		 image = imagegray;
		 return 0;
	 }

	 int HistgramBinaryzation::WellneradaptiveThreshold2(cv::Mat &image, int Radius, int Threshold)
	 {
		 
		 if (Radius < 0 || Radius > 255) return 2;
		 if (Threshold < 0 || Threshold > 100) return 2;

		 int Width, Height, Stride, X, Y;
		 int Sum, X1, X2, Y1, Y2, Y2Y1, InvertThreshold;
		 byte* Pointer;
		 Width = image.cols;
		 Height = image.rows;
		 cv::Mat imagegray ;
		 if (image.channels() == 3)
		 {
			 imagegray.create(Height, Width, CV_8UC1);
			 cvtColor(image, imagegray, CV_BGR2GRAY);
		 }
		 else
			 imagegray = image;
		
		 InvertThreshold = 100 - Threshold;
		 int* Integral = (int*)malloc(Width * Height * 4);
		 int* IndexOne, *IndexTwo;
		 for (Y = 0; Y < Height; Y++)
		 {
			 Sum = 0;
			 Pointer = imagegray.ptr<uchar>(Y);
			 IndexOne = Integral + Width * Y;
			 for (X = 0; X < Width; X++)
			 {
				 Sum += *Pointer;
				 if (Y == 0)
					 *IndexOne = Sum;
				 else
					 *IndexOne = *(IndexOne - Width) + Sum;
				 IndexOne++;
				 Pointer++;
			 }
		 }

		 for (Y = 0; Y < Height; Y++)
		 {
			 Pointer = imagegray.ptr<uchar>(Y);
			 Y1 = Y - Radius; Y2 = Y + Radius;
			 if (Y1 < 0) Y1 = 0;
			 if (Y2 >= Height) Y2 = Height - 1;
			 IndexOne = Integral + Y1 * Width;
			 IndexTwo = Integral + Y2 * Width;
			 Y2Y1 = (Y2 - Y1) * 100;
			 for (X = 0; X < Width; X++)
			 {
				 X1 = X - Radius; X2 = X + Radius;
				 if (X1 < 0) X1 = 0;
				 if (X2 >= Width) X2 = Width - 1;
				 Sum = *(IndexTwo + X2) - *(IndexOne + X2) - *(IndexTwo + X1) + *(IndexOne + X1);
				 if (*Pointer * (X2 - X1) * Y2Y1 < Sum * InvertThreshold)
					 *Pointer = 0;
				 else
					 *Pointer = 255;
				 Pointer++;
			 }
		 }
		 free(Integral);
		 image = imagegray;
	 }
//Adaptive Thresholding Using the Integral Image
#define T (0.15f)
	 int HistgramBinaryzation::adaptiveThreshold(cv::Mat &image)
	 {
		 int Width = image.cols;
		 int  Height = image.rows;
		 unsigned long* integralImg = 0;
		 int i, j;
		 long sum = 0;
		 int count = 0;
		 int index;
		 int x1, y1, x2, y2;
		 int s2 =( Width / 8 )/ 2;
		 integralImg = (unsigned long*)malloc(Width*Height*sizeof(unsigned long*));
		 cv::Mat imagegray;
		 if (image.channels() == 3)
		 {
			 imagegray.create(Height, Width, CV_8UC1);
			 cvtColor(image, imagegray, CV_BGR2GRAY);
		 }
		 else
			 imagegray = image;
		 for (i = 0; i < Width; i++)
		 {
			 // reset this column sum
			 sum = 0;

			 for (j = 0; j < Height; j++)
			 {
				 index = j*Width + i;
				 byte* Pointer = imagegray.ptr<uchar>(j);
				 sum += Pointer[i];
				 if (i == 0)
					 integralImg[index] = sum;
				 else
					 integralImg[index] = integralImg[index - 1] + sum;
			 }
		 }
		 // perform thresholding
		 for (i = 0; i < Width; i++)
		 {
			 for (j = 0; j < Height; j++)
			 {
				 index = j*Width + i;

				 // set the SxS region
				 x1 = i - s2; x2 = i + s2;
				 y1 = j - s2; y2 = j + s2;

				 // check the border
				 if (x1 < 0) x1 = 0;
				 if (x2 >= Width) x2 = Width - 1;
				 if (y1 < 0) y1 = 0;
				 if (y2 >= Height) y2 = Height - 1;

				 count = (x2 - x1)*(y2 - y1);
				 byte* Pointer = imagegray.ptr<uchar>(j);
				 // I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
				 sum = integralImg[y2*Width + x2] -
					 integralImg[y1*Width + x2] -
					 integralImg[y2*Width + x1] +
					 integralImg[y1*Width + x1];

				 if ((long)(Pointer[i] * count) < (long)(sum*(1.0 - T)))
					 Pointer[i] = 0;
				 else
					 Pointer[i] = 255;
			 }
		 }
		 image = imagegray;
		 free(integralImg);
		 return 0;
	 }

	 int  HistgramBinaryzation::GetHuangFuzzyThreshold(int* HistGram, int nSize)
	 {
		 int X, Y;
		 int First, Last;
		 int Threshold = -1;
		 double BestEntropy = 1.7e+308, Entropy;
		 //   找到第一个和最后一个非0的色阶值
		 for (First = 0; First <nSize && HistGram[First] == 0; First++);
		 for (Last = nSize - 1; Last > First && HistGram[Last] == 0; Last--);
		 if (First == Last) return First;                // 图像中只有一个颜色
		 if (First + 1 == Last) return First;            // 图像中只有二个颜色

		 // 计算累计直方图以及对应的带权重的累计直方图
		 int* S = new int[Last + 1];
		 int* W = new int[Last + 1];            // 对于特大图，此数组的保存数据可能会超出int的表示范围，可以考虑用long类型来代替
		 memset(S, 0, sizeof(int)*(Last + 1));
		 memset(W, 0, sizeof(int)*(Last + 1));
		 S[0] = HistGram[0];
		 for (Y = First > 1 ? First : 1; Y <= Last; Y++)
		 {
			 S[Y] = S[Y - 1] + HistGram[Y];
			 W[Y] = W[Y - 1] + Y * HistGram[Y];
		 }

		 // 建立公式（4）及（6）所用的查找表
		 int nSizeSmu = Last + 1 - First;
		 double* Smu = new double[nSizeSmu];
		 memset(Smu, 0, sizeof(double)*nSizeSmu);
		 for (Y = 1; Y < nSizeSmu; Y++)
		 {
			 double mu = 1 / (1 + (double)Y / (Last - First));               // 公式（4）
			 Smu[Y] = -mu * log(mu) - (1 - mu) * log(1 - mu);      // 公式（6）
		 }

		 // 迭代计算最佳阈值
		 for (Y = First; Y <= Last; Y++)
		 {
			 Entropy = 0;
			 int mu = (int)round((double)W[Y] / S[Y]);             // 公式17
			 for (X = First; X <= Y; X++)
				 Entropy += Smu[abs(X - mu)] * HistGram[X];
			 mu = (int)round((double)(W[Last] - W[Y]) / (S[Last] - S[Y]));  // 公式18
			 for (X = Y + 1; X <= Last; X++)
				 Entropy += Smu[abs(X - mu)] * HistGram[X];       // 公式8
			 if (BestEntropy > Entropy)
			 {
				 BestEntropy = Entropy;      // 取最小熵处为最佳阈值
				 Threshold = Y;
			 }
		 }
		 delete[] Smu;
		 delete[] S;
		 delete[] W;
		 return Threshold;
	 }
	 void HistgramBinaryzation::computeHistogram(cv::Mat &input, int *hist)
	 {
		 int Width = input.cols;
		 int Height = input.rows;
		 for (int i = 0; i <= 255; i++) hist[i] = 0;
		 for (int Y = 0; Y < Height; Y++)
		 {
			 unsigned char* LinePD = input.ptr<uchar>(Y);
			 for (int X = 0; X < Width; X++)
			 {
				 int value = (int)LinePD[X];
				 hist[value]++;
			 }
		 }
	 }
	 int HistgramBinaryzation::HuangFuzzyThreshold(cv::Mat &image)
	 {
		 int Width = image.cols;
		 int  Height = image.rows;		 
		 cv::Mat imagegray;
		 if (image.channels() == 3)
		 {
			 imagegray.create(Height, Width, CV_8UC1);
			 cvtColor(image, imagegray, CV_BGR2GRAY);
		 }
		 else
			 imagegray = image;
		 int HistGram[256] = { 0 };
		 computeHistogram(imagegray, HistGram);
		 int threshold= GetHuangFuzzyThreshold(HistGram, 256);
		 for (int Y = 0; Y < Height; Y++)
		 {
			 unsigned char* LinePD = imagegray.ptr<uchar>(Y);
			 for (int X = 0; X < Width; X++)
			 {
				 int value = (int)LinePD[X];
				 if (value > threshold){
					 LinePD[X] = 255.0f;
				 }
				 else{
					 LinePD[X] = 0.0f;
				 }
			 }
		 }
		 image = imagegray;
		 return 0;
	 }