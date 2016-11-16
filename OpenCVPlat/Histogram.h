#pragma once
class Histogram
{
public:
	Histogram();
	~Histogram();

private:
	int iHistogramR[256];
	int iHistogramG[256];
	int iHistogramB[256];
	int iHistogramBW[256];

	int iCumHistogramR[256];
	int iCumHistogramG[256];
	int iCumHistogramB[256];
	int iCumHistogramBW[256];
};

