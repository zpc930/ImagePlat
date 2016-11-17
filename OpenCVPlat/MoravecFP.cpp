#include "stdafx.h"
#include "MoravecFP.h"


MoravecFP::MoravecFP()
{
}


MoravecFP::~MoravecFP()
{
}
void InterestValue(int *a, int &v)     //得到5*5区域模板的兴趣值
{
	int v1, v2, v3, v4;
	v1 = v2 = v3 = v4 = 0;
	v1 = (a[11] - a[10])*(a[11] - a[10]) + (a[12] - a[11])*(a[12] - a[11]) + (a[13] - a[12])*(a[13] - a[12]) + (a[14] - a[13])*(a[14] - a[13]);
	v2 = (a[6] - a[0])*(a[6] - a[0]) + (a[12] - a[6])*(a[12] - a[6]) + (a[18] - a[12])*(a[18] - a[12]) + (a[24] - a[18])*(a[24] - a[18]);
	v3 = (a[7] - a[2])*(a[7] - a[2]) + (a[12] - a[7])*(a[12] - a[7]) + (a[17] - a[12])*(a[17] - a[12]) + (a[22] - a[17])*(a[22] - a[17]);
	v4 = (a[8] - a[4])*(a[8] - a[4]) + (a[12] - a[8])*(a[12] - a[8]) + (a[16] - a[12])*(a[16] - a[12]) + (a[20] - a[16])*(a[20] - a[16]);
	v = v1;
	if (v > v2)
		v = v2;
	if (v > v3)
		v = v3;
	if (v > v4)
		v = v4;
}
void getmax(int *m_a, int &m_max, int &m_num)      //得到7*7区域的候选特征值
{
	m_max = m_a[0];
	for (int i = 1; i < 49; i++)
	{
		if (m_max < m_a[i])
		{
			m_max = m_a[i];
			m_num = i;
		}
	}
}
int MoravecFP::MaravecProcess(cv::Mat& f_image)
{
	int thresh = 400;					//阈值
	int nWidth = f_image.cols;
	int nHeight = f_image.rows;
	int  *temp = new  int[nHeight*nWidth];
	int a[25];            //5*5模板
	int b[49];            //7*7区域
	int m_k=0;		//特征点数量；
	int *tempfp = new int[nHeight*nWidth];

	int *tempfp1 = tempfp;
	int i, j;
	for (int i = 0; i < nHeight;i++)
	{
		for (int j = 0; j < nWidth;j++)
		{

			if (i<2 || i>nHeight - 3 || j<2 || j>nWidth - 3)
			{
				temp[i*nWidth + j] = 0;
			}
			else
			{
				uchar *data = f_image.ptr<uchar>(i-2);
				a[0] = data[j - 2];
				a[1] = data[j - 1];
				a[2] = data[j];
				a[3] = data[j + 1];
				a[4] = data[j + 2];
				data = f_image.ptr<uchar>(i -1);
				a[5] = data[j - 2];
				a[6] = data[ j- 1];
				a[7] = data[ j];
				a[8] = data[ j + 1];
				a[9] = data[j+ 2];
				data = f_image.ptr<uchar>(i);
				a[10] = data[ j - 2];
				a[11] = data[ j - 1];
				a[12] = data[ j];
				a[13] = data[j + 1];
				a[14] = data[ j + 2];
				data = f_image.ptr<uchar>(i+1);
				a[15] = data[ j- 2];
				a[16] = data[j  - 1];
				a[17] = data[j ];
				a[18] = data[ j +  1];
				a[19] = data[ j +  2];
				data = f_image.ptr<uchar>(i + 2);
				a[20] = data[ j- 2];
				a[21] = data[ j - 1];
				a[22] = data[ j ];
				a[23] = data[ j + 1];
				a[24] = data[ j +  2];
				InterestValue(a, temp[i*nWidth + j]);
			}
		}
	}
	int m_w1 = nWidth / 7;
	int m_w2 = nWidth % 7;
	int m_h1 = nHeight / 7;
	int m_h2 = nHeight % 7;
	int m_r = 0;
	int m_c = 0;
	for (i = 0; i < m_h1; i++)
	{
		for (int j = 0; j < m_w1; j++)
		{
			for (int k = 0; k < 7; k++)
			{
				b[7 * k] = temp[i*nWidth * 7 + j * 7 + k*nWidth];
				b[7 * k + 1] = temp[i*nWidth * 7 + j * 7 + 1 + k*nWidth];
				b[7 * k + 2] = temp[i*nWidth * 7 + j * 7 + 2 + k*nWidth];
				b[7 * k + 3] = temp[i*nWidth * 7 + j * 7 + 3 + k*nWidth];
				b[7 * k + 4] = temp[i*nWidth * 7 + j * 7 + 4 + k*nWidth];
				b[7 * k + 5] = temp[i*nWidth * 7 + j * 7 + 5 + k*nWidth];
				b[7 * k + 6] = temp[i*nWidth * 7 + j * 7 + 6 + k*nWidth];
			}
			int m_tempnum = 0;
			int m_num = 0;
			getmax(b, m_tempnum, m_num);				//取得局部最大值
			if (m_tempnum > thresh)                   //局部最大值大于某一阈值确定为角点
			{
				m_r = m_num / 7;
				m_c = m_num % 7;
				m_r = 7 * i + m_r;
				m_c = 7 * j + m_c;
				*(tempfp1++) = m_r;
				*(tempfp1++) = m_c;
				m_k++;
			}
		}
	}
	/*m_fp = new int[2 * m_k];
	memcpy(m_fp, tempfp, 4 * 2 * m_k)*/;
	delete[] tempfp;
	delete[] temp;

	return 0;
}