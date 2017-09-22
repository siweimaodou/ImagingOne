#pragma once
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

class ImageProcessManagement
{
public:
	ImageProcessManagement();
	~ImageProcessManagement();

	bool paddingImage(const string& imageSrcFile, const string& imageDstFile);

	void filling(Mat& dstImage, cv::Rect maxRect, unsigned int  mean, float sigma);
	void pixelFillSpecial(Mat& dstImage, int row, int col, unsigned int  mean, float sigma);
	void pixelFill(Mat& dstImage, int row, int col, unsigned int  mean, float sigma);
	void myMedianBlur(Mat dstImage, cv::Rect maxRect, const unsigned char backgroundMean);
};

