#include "stdafx.h"
#include "HistgramBinaryzation.h"


HistgramBinaryzation::HistgramBinaryzation()
{
}


HistgramBinaryzation::~HistgramBinaryzation()
{
}
//�Ҷ�ƽ��ֵֵ����
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
//�ٷֱ���ֵ��P-Tile����
 int HistgramBinaryzation::GetPTileThreshold(int* HistGram, int Tile , int nSize)
{
	int Y, Amount = 0, Sum = 0;
	for (Y = 0; Y < nSize; Y++) Amount += HistGram[Y];        //  ��������
	for (Y = 0; Y < nSize; Y++)
	{
		Sum = Sum + HistGram[Y];
		if (Sum >= Amount * Tile / 100) return Y;
	}
	return -1;
}
 /*�������ڹȵ���Сֵ����ֵ

	 1��������

	 �˷���ʵ���ھ�������˫��ֱ��ͼ��ͼ����Ѱ��˫��Ĺȵ���Ϊ��ֵ�����Ǹ÷�����һ���ܻ����ֵ��������Щ����ƽ̹��ֱ��ͼ�򵥷�ͼ�񣬸÷��������ʡ�

	 2��ʵ�ֹ��̣�

	 �ú�����ʵ����һ�������Ĺ��̣�ÿ�δ���ǰ��ֱ��ͼ���ݽ����жϣ������Ƿ��Ѿ���һ��˫���ֱ��ͼ��������ǣ����ֱ��ͼ���ݽ��а뾶Ϊ1�����ڴ�СΪ3����ƽ�������������һ������������1000�κ���δ���һ��˫���ֱ��ͼ������ִ��ʧ�ܣ���ɹ���ã���������ֵȡ����˫��֮��Ĺȵ�ֵ��Ϊ��ֵ��

	 ע���ڱ�������У�ƽ���Ĵ�����Ҫ��ǰ����֮ǰ����Ϣ�������Ҫ��ƽ��ǰ�����ݽ���һ�����ݡ����⣬���������;������ƣ���Ӧ�����ε�ֱ��ͼ���ݣ�����ת��Ϊ�����������������д�������ò�����ȷ�Ľ����
 */
  static bool IsDimodal(double* HistGram,int nSize)       // ���ֱ��ͼ�Ƿ�Ϊ˫���
 {
	 // ��ֱ��ͼ�ķ���м�����ֻ�з���λ2��Ϊ˫�� 
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
	 double* HistGramC = new double[nSize];           // ���ھ������⣬һ��Ҫ�ø���������������ò�����ȷ�Ľ��
	 double* HistGramCC = new double[nSize];          // ���ֵ�Ĺ��̻��ƻ�ǰ������ݣ������Ҫ��������
	 for (Y = 0; Y < nSize; Y++)
	 {
		 HistGramC[Y] = HistGram[Y];
		 HistGramCC[Y] = HistGram[Y];
	 }

	 // ͨ���������ֵ��ƽ��ֱ��ͼ
	 while (IsDimodal(HistGramCC,nSize) == false)                                        // �ж��Ƿ��Ѿ���˫���ͼ����      
	 {
		 HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;                 // ��һ��
		 for (Y = 1; Y < nSize; Y++)
			 HistGramCC[Y] = (HistGramC[Y - 1] + HistGramC[Y] + HistGramC[Y + 1]) / 3;     // �м�ĵ�
		 HistGramCC[nSize - 1] = (HistGramC[nSize - 2] + HistGramC[nSize - 1] + HistGramC[nSize-1]) / 3;         // ���һ��
		 memcpy(HistGramCC, HistGramC, nSize * sizeof(double));
		 Iter++;
		 if (Iter >= 1000) return -1;                                                   // ֱ��ͼ�޷�ƽ��Ϊ˫��ģ����ش������
	 }
	 // ��ֵ��Ϊ����֮�����Сֵ 
	 bool Peakfound = false;
	 for (Y = 1; Y < nSize-1; Y++)
	 {
		 if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peakfound = true;
		 if (Peakfound == true && HistGramCC[Y - 1] >= HistGramCC[Y] && HistGramCC[Y + 1] >= HistGramCC[Y])
			 return Y - 1;
	 }
	 return -1;
 }
  //�ġ�����˫��ƽ��ֵ����ֵ
//	  1��������
//  ���㷨�ͻ��ڹȵ���Сֵ����ֵ�������ƣ�ֻ�����һ������ȡ��˫��֮��Ĺȵ�ֵ������ȡ˫���ƽ��ֵ��Ϊ��ֵ��
  static int GetIntermodesThreshold(int* HistGram,int nSize)
  {
	  int Y, Iter = 0, Index;
	  double* HistGramC = new double[nSize];           // ���ھ������⣬һ��Ҫ�ø���������������ò�����ȷ�Ľ��
	  double* HistGramCC = new double[nSize];          // ���ֵ�Ĺ��̻��ƻ�ǰ������ݣ������Ҫ��������
	  for (Y = 0; Y < nSize; Y++)
	  {
		  HistGramC[Y] = HistGram[Y];
		  HistGramCC[Y] = HistGram[Y];
	  }
	  // ͨ���������ֵ��ƽ��ֱ��ͼ
	  while (IsDimodal(HistGramCC,nSize) == false)                                                  // �ж��Ƿ��Ѿ���˫���ͼ����      
	  {
		  HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;                   // ��һ��
		  for (Y = 1; Y < nSize-1; Y++)
			  HistGramCC[Y] = (HistGramC[Y - 1] + HistGramC[Y] + HistGramC[Y + 1]) / 3;       // �м�ĵ�
		  HistGramCC[nSize - 1] = (HistGramC[nSize - 2] + HistGramC[nSize - 1] + HistGramC[nSize-1]) / 3;           // ���һ��
		  memcpy(HistGramCC, HistGramC, nSize * sizeof(double));         // �������ݣ�Ϊ��һ�ε�����׼��
		  Iter++;
		  if (Iter >= 10000) return -1;                                                       // �ƺ�ֱ��ͼ�޷�ƽ��Ϊ˫��ģ����ش������
	  }
	  // ��ֵΪ����ֵ��ƽ��ֵ
	  int* Peak = new int[2];
	  for (Y = 1, Index = 0; Y < nSize-1; Y++)
	  if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peak[Index++] = Y - 1;
	  int Threshold = ((Peak[0] + Peak[1]) / 2);
	  delete[] Peak;
	  delete[] HistGramC;
	  delete[] HistGramCC;
	  return Threshold;
  }
 /* �塢���������ֵ

	  1��������

	  ���㷨�ȼٶ�һ����ֵ��Ȼ������ڸ���ֵ�µ�ǰ���ͱ���������ֵ����ǰ���ͱ�������ֵ��ƽ��ֵ�ͼٶ�����ֵ��ͬʱ���������ֹ�����Դ�ֵΪ��ֵ���ж�ֵ����

	  2��ʵ�ֹ��̣�

	  ��1�����ͼ������Ҷ�ֵ����С�Ҷ�ֵ���ֱ��Ϊgl��gu�����ʼ��ֵΪ��



	  (2) ������ֵT0��ͼ��ָ�Ϊǰ���ͱ������ֱ�������ߵ�ƽ���Ҷ�ֵAb��Af:



  (3) ��



	  ���Tk = Tk + 1, ��ȡTkΪ����õ���ֵ������ת2����������*/
   int GetIterativeBestThreshold(int* HistGram,int nSize)
  {
	  int X, Iter = 0;
	  int MeanValueOne, MeanValueTwo, SumOne, SumTwo, SumIntegralOne, SumIntegralTwo;
	  int MinValue, MaxValue;
	  int Threshold, NewThreshold;

	  for (MinValue = 0; MinValue < nSize && HistGram[MinValue] == 0; MinValue++);
	  for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);

	  if (MaxValue == MinValue) return MaxValue;          // ͼ����ֻ��һ����ɫ             
	  if (MinValue + 1 == MaxValue) return MinValue;      // ͼ����ֻ�ж�����ɫ

	  Threshold = MinValue;
	  NewThreshold = (MaxValue + MinValue) >> 1;
	  while (Threshold != NewThreshold)    // ��ǰ�����ε����Ļ����ֵ��ͬʱ����������    
	  {
		  SumOne = 0; SumIntegralOne = 0;
		  SumTwo = 0; SumIntegralTwo = 0;
		  Threshold = NewThreshold;
		  for (X = MinValue; X <= Threshold; X++)         //������ֵ��ͼ��ָ��Ŀ��ͱ��������֣���������ֵ�ƽ���Ҷ�ֵ      
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
		  NewThreshold = (MeanValueOne + MeanValueTwo) >> 1;       //����µ���ֵ
		  Iter++;
		  if (Iter >= 1000) return -1;
	  }
	  return Threshold;
  }
   /*����OSTU���ɷ�

����1��������

  ���㷨��1979�����ձ��������ģ���Ҫ��˼����ȡĳ����ֵ��ʹ��ǰ���ͱ����������䷽�����matlab�е�graythresh�����Ը��㷨Ϊԭ��ִ�еġ�

  2��ԭ��

  ���ڸ��㷨��ԭ���������кܶ࣬����Ϊ��ƪ�����ޣ�������׸����

  3���ο����룺*/
    int GetOSTUThreshold(int* HistGram,int nSize)
   {
	   int  Y, Amount = 0;
	   int PixelBack = 0, PixelFore = 0, PixelIntegralBack = 0, PixelIntegralFore = 0, PixelIntegral = 0;
	   double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma;              // ��䷽��;
	   int MinValue, MaxValue;
	   int Threshold = 0;

	   for (MinValue = 0; MinValue < nSize && HistGram[MinValue] == 0; MinValue++);
	   for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);
	   if (MaxValue == MinValue) return MaxValue;          // ͼ����ֻ��һ����ɫ             
	   if (MinValue + 1 == MaxValue) return MinValue;      // ͼ����ֻ�ж�����ɫ

	   for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  ��������

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
	/* �ߡ�һά�����

	  1��������

	  ���㷨����Ϣ�����صĸ������뵽ͼ���У�ͨ��������ֵ�ָ���������صĺ����ж���ֵ�Ƿ�Ϊ�����ֵ��

	  2���㷨ԭ��

	  �ⷽ�������Ҳ�Ƚ϶࣬������������ȥ����������ϡ�*/
	int Get1DMaxEntropyThreshold(int* HistGram)
	{
		int  X, Y, Amount = 0;
		double* HistGramD = new double[256];
		double SumIntegral, EntropyBack, EntropyFore, MaxEntropy;
		int MinValue = 255, MaxValue = 0;
		int Threshold = 0;

		for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++);
		for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--);
		if (MaxValue == MinValue) return MaxValue;          // ͼ����ֻ��һ����ɫ             
		if (MinValue + 1 == MaxValue) return MinValue;      // ͼ����ֻ�ж�����ɫ

		for (Y = MinValue; Y <= MaxValue; Y++) Amount += HistGram[Y];        //  ��������

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
	/*�ˡ����ر��ַ�
		1������:

	���㷨ͨ��ѡ��ǡ������ֵ�Ӷ�ʹ�ö�ֵ���ͼ���ԭʼ�ĻҶ�ͼ�����������ͬ�ĳ�ʼ����ֵ��

		2��ԭ��

		�ο����ģ�W.Tsai, ��Moment - preserving thresholding : a new approach, �� Comput.Vision Graphics Image Process., vol. 29, pp. 377 - 393, 1985.

		�����޷����ص������ģ��շѵģ�����������������һЩ�������ҵ��Ĺ�ʽ����һ�¡�



		���е�A\B\C�ĺ����ɼ����벿�֡�*/
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

		for (Y = 0; Y <= 255; Y++) Amount += HistGram[Y];        //  ��������
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
		 //   �ҵ���һ�������һ����0��ɫ��ֵ
		 for (First = 0; First <nSize && HistGram[First] == 0; First++);
		 for (Last = nSize - 1; Last > First && HistGram[Last] == 0; Last--);
		 if (First == Last) return First;                // ͼ����ֻ��һ����ɫ
		 if (First + 1 == Last) return First;            // ͼ����ֻ�ж�����ɫ

		 // �����ۼ�ֱ��ͼ�Լ���Ӧ�Ĵ�Ȩ�ص��ۼ�ֱ��ͼ
		 int* S = new int[Last + 1];
		 int* W = new int[Last + 1];            // �����ش�ͼ��������ı������ݿ��ܻᳬ��int�ı�ʾ��Χ�����Կ�����long����������
		 memset(S, 0, sizeof(int)*(Last + 1));
		 memset(W, 0, sizeof(int)*(Last + 1));
		 S[0] = HistGram[0];
		 for (Y = First > 1 ? First : 1; Y <= Last; Y++)
		 {
			 S[Y] = S[Y - 1] + HistGram[Y];
			 W[Y] = W[Y - 1] + Y * HistGram[Y];
		 }

		 // ������ʽ��4������6�����õĲ��ұ�
		 int nSizeSmu = Last + 1 - First;
		 double* Smu = new double[nSizeSmu];
		 memset(Smu, 0, sizeof(double)*nSizeSmu);
		 for (Y = 1; Y < nSizeSmu; Y++)
		 {
			 double mu = 1 / (1 + (double)Y / (Last - First));               // ��ʽ��4��
			 Smu[Y] = -mu * log(mu) - (1 - mu) * log(1 - mu);      // ��ʽ��6��
		 }

		 // �������������ֵ
		 for (Y = First; Y <= Last; Y++)
		 {
			 Entropy = 0;
			 int mu = (int)round((double)W[Y] / S[Y]);             // ��ʽ17
			 for (X = First; X <= Y; X++)
				 Entropy += Smu[abs(X - mu)] * HistGram[X];
			 mu = (int)round((double)(W[Last] - W[Y]) / (S[Last] - S[Y]));  // ��ʽ18
			 for (X = Y + 1; X <= Last; X++)
				 Entropy += Smu[abs(X - mu)] * HistGram[X];       // ��ʽ8
			 if (BestEntropy > Entropy)
			 {
				 BestEntropy = Entropy;      // ȡ��С�ش�Ϊ�����ֵ
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