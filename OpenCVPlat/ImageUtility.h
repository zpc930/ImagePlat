#pragma once
namespace cv
{
	template<typename T> static void
		split_(const T* src, T** dst, int len, int cn);
	template<typename T> static void
		merge_(const T** src, T* dst, int len, int cn);
}


