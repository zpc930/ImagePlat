#pragma once

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

class ColorConvert
{
public:

public:
	static void RGBtoHSV(float r, float g, float b, float *h, float *s, float *v);
	static void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v);
};