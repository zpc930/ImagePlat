#include "stdafx.h"
#include "TVDenoising.h"
#include <algorithm>

TVDenoising::TVDenoising()
{
}


TVDenoising::~TVDenoising()
{
}
//开辟二维数组函数
double** TVDenoising::newDoubleMatrix(int nx, int ny)
{
	double** matrix = new double*[ny];

	for (int i = 0; i < ny; i++)
	{
		matrix[i] = new double[nx];
	}
	if (!matrix)
		return NULL;
	return
		matrix;
}
//清除二维数组内存函数
bool TVDenoising::deleteDoubleMatrix(double** matrix, int nx, int ny)
{
	if (!matrix)
	{
		return true;
	}
	for (int i = 0; i < ny; i++)
	{
		if (matrix[i])
		{
			delete[] matrix[i];
		}
	}
	delete[] matrix;

	return true;
}
int TVDenoising::ProcessTVD(cv::Mat &imageIn, int iter)
{
	int Width = imageIn.cols;
	int Height = imageIn.rows;
	cv::Mat imagegray;
	if (imageIn.channels() == 3)
	{
		imagegray.create(Height, Width, CV_8UC1);
		cvtColor(imageIn, imagegray, CV_BGR2GRAY);
	}
	else
		imagegray = imageIn;
	//基本参数，这里由于设置矩阵C为0矩阵，不参与运算，所以就忽略之
	int ep = 1, nx = Width, ny = Height;
	double dt = (double)ep / 5.0f, lam = 0.0;
	int ep2 = ep*ep;
	double** image = newDoubleMatrix(nx, ny);
	double** image0 = newDoubleMatrix(nx, ny);
	//注意一点是CxImage里面图像存储的坐标原点是左下角，Matlab里面图像时左上角原点
	for (int i = 0; i < ny; i++)
	{
		uchar *data = imagegray.ptr<uchar>(i);
		for (int j = 0; j < nx; j++)
		{
			image0[i][j] = image[i][j] = data[j];
		}
	}
	double** image_x = newDoubleMatrix(nx, ny);   //I_x = ( I(:,[2:nx nx]) - I(:,[1 1:nx-1]))/2;
	double** image_xx = newDoubleMatrix(nx, ny);   //I_xx = I(:,[2:nx nx])+I(:,[1 1:nx-1])-2*I;
	double** image_y = newDoubleMatrix(nx, ny);   //I_y = (I([2:ny ny],:)-I([1 1:ny-1],:))/2;
	double** image_yy = newDoubleMatrix(nx, ny);   //I_yy = I([2:ny ny],:)+I([1 1:ny-1],:)-2*I;
	double** image_tmp1 = newDoubleMatrix(nx, ny);
	double** image_tmp2 = newDoubleMatrix(nx, ny);

	double** image_dp = newDoubleMatrix(nx, ny);   //Dp = I([2:ny ny],[2:nx nx])+I([1 1:ny-1],[1 1:nx-1

	double** image_dm = newDoubleMatrix(nx, ny);   //Dm = I([1 1:ny-1],[2:nx nx])+I([2:ny ny],[1 1:nx-1]);

	double** image_xy = newDoubleMatrix(nx, ny);   //I_xy = (Dp-Dm)/4;

	double** image_num = newDoubleMatrix(nx, ny);   //Num = I_xx.*(ep2+I_y.^2)-2*I_x.*I_y.*I_xy+I_yy.*(ep2+I_x.^2);
	double** image_den = newDoubleMatrix(nx, ny);   //Den = (ep2+I_x.^2+I_y.^2).^(3/2);
	//对image进行迭代iter次
	//////////////////////////////////////////////////////////////////////////
	//对image进行迭代iter次
	iter = 80;
	for (int t = 1; t <= iter; t++)
	{
				//////////////////////////////////////////////////////////////////////////
		//计算I(:,[2:nx nx])和I(:,[1 1:nx-1])
		//公共部分2到nx-1列
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx - 1; j++)
			{
				image_tmp1[i][j] = image[i][j + 1];
				image_tmp2[i][j + 1] = image[i][j];
			}
		}
		for (int i = 0; i < ny; i++)
		{
			image_tmp1[i][nx - 1] = image[i][nx - 1];
			image_tmp2[i][0] = image[i][0];
		}

		//计算I_x, I_xx
		// I_x = ( I(:,[2:nx nx]) - I(:,[1 1:nx-1]))/2
		//I_xx = I(:,[2:nx nx])+I(:,[1 1:nx-1])-2*I;
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_x[i][j] = (image_tmp1[i][j] - image_tmp2[i][j]) / 2;
				image_xx[i][j] = (image_tmp1[i][j] + image_tmp2[i][j]) - 2 * image[i][j];
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//计算I([2:ny ny],:)和I([1 1:ny-1],:)
		//公共部分2到ny-1行
		for (int i = 0; i < ny - 1; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_tmp1[i][j] = image[i + 1][j];
				image_tmp2[i + 1][j] = image[i][j];
			}
		}
		for (int j = 0; j < nx; j++)
		{
			image_tmp1[ny - 1][j] = image[ny - 1][j];
			image_tmp2[0][j] = image[0][j];
		}
		//计算I_xx, I_yy
		// I_y = I([2:ny ny],:)-I([1 1:ny-1],:)
		//I_yy = I([2:ny ny],:)+I([1 1:ny-1],:)-2*I;
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_y[i][j] = (image_tmp1[i][j] - image_tmp2[i][j]) / 2;
				image_yy[i][j] = (image_tmp1[i][j] + image_tmp2[i][j]) - 2 * image[i][j];
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//计算I([2:ny ny],[2:nx nx])和I([1 1:ny-1],[1 1:nx-1])
		//公共部分分别是矩阵右下角，左上角的ny-1行和nx-1列
		for (int i = 0; i < ny - 1; i++)
		{
			for (int j = 0; j < nx - 1; j++)
			{
				image_tmp1[i][j] = image[i + 1][j + 1];
				image_tmp2[i + 1][j + 1] = image[i][j];
			}
		}
		for (int i = 0; i < ny - 1; i++)
		{
			image_tmp1[i][nx - 1] = image[i + 1][nx - 1];
			image_tmp2[i + 1][0] = image[i][0];
		}
		for (int j = 0; j < nx - 1; j++)
		{
			image_tmp1[ny - 1][j] = image[ny - 1][j + 1];
			image_tmp2[0][j + 1] = image[0][j];
		}
		image_tmp1[ny - 1][nx - 1] = image[ny - 1][nx - 1];
		image_tmp2[0][0] = image[0][0];
		//计算Dp = I([2:ny ny],[2:nx nx])+I([1 1:ny-1],[1 1:nx-1]);
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_dp[i][j] = image_tmp1[i][j] + image_tmp2[i][j];
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//计算I([1 1:ny-1],[2:nx nx])和I([2:ny ny],[1 1:nx-1])
		//公共部分分别是矩阵左下角，右上角的ny-1行和nx-1列
		for (int i = 0; i < ny - 1; i++)
		{
			for (int j = 0; j < nx - 1; j++)
			{
				image_tmp1[i + 1][j] = image[i][j + 1];
				image_tmp2[i][j + 1] = image[i + 1][j];
			}
		}
		for (int i = 0; i < ny - 1; i++)
		{
			image_tmp1[i + 1][nx - 1] = image[i][nx - 1];
			image_tmp2[i][0] = image[i + 1][0];
		}
		for (int j = 0; j < nx - 1; j++)
		{
			image_tmp1[0][j] = image[0][j + 1];
			image_tmp2[ny - 1][j + 1] = image[ny - 1][j];
		}
		image_tmp1[0][nx - 1] = image[0][nx - 1];
		image_tmp2[ny - 1][0] = image[ny - 1][0];

		//计算Dm = I([1 1:ny-1],[2:nx nx])+I([2:ny ny],[1 1:nx-1]);
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_dm[i][j] = image_tmp1[i][j] + image_tmp2[i][j];
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//计算I_xy = (Dp-Dm)/4;
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_xy[i][j] = (image_dp[i][j] - image_dm[i][j]) / 4;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//计算过程：

		//计算Num = I_xx.*(ep2+I_y.^2)-2*I_x.*I_y.*I_xy+I_yy.*(ep2+I_x.^2) 和 Den = (ep2+I_x.^2+I_y.^2).^(3/2);
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image_num[i][j] = image_xx[i][j] * (image_y[i][j] * image_y[i][j] + ep2)
					- 2 * image_x[i][j] * image_y[i][j] * image_xy[i][j] + image_yy[i][j] * (image_x[i][j] * image_x[i][j] + ep2);

				image_den[i][j] = pow((image_x[i][j] * image_x[i][j] + image_y[i][j] * image_y[i][j] + ep2), 1.5);
			}
		}

		//计算I: I_t = Num./Den + lam.*(I0-I+C); I=I+dt*I_t;  %% evolve image by dt
		for (int i = 0; i < ny; i++)
		{
			for (int j = 0; j < nx; j++)
			{
				image[i][j] += dt*(image_num[i][j] / image_den[i][j] + lam*(image0[i][j] - image[i][j]));
			}
		}
	}
	//迭代结束

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
#define max(a,b)  (a) >(b) ? (a) : (b)
#define min(a,b)  (a) <(b) ? (a) : (b)
	//赋值图像
	BYTE tmp;
	for (int i = 0; i < ny; i++)
	{
		uchar *data = imagegray.ptr<uchar>(i);
		for (int j = 0; j < nx; j++)
		{
			tmp = (BYTE)image[i][j];
			tmp = max(0, min(tmp, 255));			
			data[j] = tmp;
				
			
        //	my_image->SetPixelIndex(j, ny - i - 1, tmp);
		}
	}
	imageIn = imagegray;
	//////////////////////////////////////////////////////////////////////////
	//删除内存
	deleteDoubleMatrix(image_x, nx, ny);
	deleteDoubleMatrix(image_y, nx, ny);
	deleteDoubleMatrix(image_xx, nx, ny);
	deleteDoubleMatrix(image_yy, nx, ny);
	deleteDoubleMatrix(image_tmp1, nx, ny);
	deleteDoubleMatrix(image_tmp2, nx, ny);
	deleteDoubleMatrix(image_dp, nx, ny);
	deleteDoubleMatrix(image_dm, nx, ny);
	deleteDoubleMatrix(image_xy, nx, ny);
	deleteDoubleMatrix(image_num, nx, ny);
	deleteDoubleMatrix(image_den, nx, ny);
	deleteDoubleMatrix(image0, nx, ny);
	deleteDoubleMatrix(image, nx, ny);
	return 0;
}