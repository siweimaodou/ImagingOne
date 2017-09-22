#pragma once
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

class HistoManagement
{
public:
	HistoManagement();
	~HistoManagement();

	unsigned char getTopThresh(MatND histo);
	bool histoIncludeTwoPeak(MatND& histo, unsigned char highThresh);
	void myCal_Hist(Mat Gray_img, MatND& hist);
	void ostuHistogramWithoutWhiteBoarder(MatND histo, unsigned char maxThresh, unsigned int& backgroundMean, float& backgroundSigma);
};

