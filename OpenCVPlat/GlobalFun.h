#pragma once


class GlobalFun
{
public:
	GlobalFun();
	~GlobalFun();
	static bool IsValidABC_Original(float A, float B, float C);
	static int Devignetting_Original(unsigned char *Src, unsigned char *Dest, int Width, int Height);
};

