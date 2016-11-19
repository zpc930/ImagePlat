#pragma once
class BoxFilter2
{
public:
	BoxFilter2();
	~BoxFilter2();
	void init(int width, int height, int mwidth = 5, int mheight = 5);
	void boxfilter(unsigned char* img);
public:
	float    getMean(int x, int y);    //以x,y为中心点，mwidth,mheight为直径的局部区域,下同
	float    getVar(int x, int y);
	int        getSum(int x, int y);
	int        getSquareSum(int x, int y);
	int        getLocalSize();

private:
	int mwidth;
	int mheight;
	unsigned char* img;
	int width;
	int height;
	int* f_sum;
	int* f_sum2;
	int* buff ;
	int* buff2;
	int boxwidth;
	int boxheight;
};

