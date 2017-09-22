#include "ImageProcessManagement.h"

#include "HistoManagement.h"
#include "LogManagement.h"
#include "FolderFileManagement.h"


const string LOG_IMAGE_PROCESS_PARAM = "imageProcessParam.txt";
const string LOG_PADDING_FILE = "paddingFiles.txt";

ImageProcessManagement::ImageProcessManagement()
{
}


ImageProcessManagement::~ImageProcessManagement()
{
}


bool ImageProcessManagement::paddingImage(const string& imageSrcFile, const string& imageDstFile)
{
	Mat srcImage, dstImage, binaryImage, morphologyImage;
	srcImage = imread(imageSrcFile);
	cvtColor(srcImage, dstImage, CV_RGB2GRAY);//把图片转化为灰度图

	//	imwrite("55.jpg", dstImage);
	//step1 histogram
	MatND hist2;
	HistoManagement histoManagementInstance;
	histoManagementInstance.myCal_Hist(dstImage, hist2);
	int highThresh = histoManagementInstance.getTopThresh(hist2);

	//直方图范围，峰值数计算
	bool flag = true;
	flag = histoManagementInstance.histoIncludeTwoPeak(hist2, highThresh);

	//step2 binary with threshold = 253
	threshold(dstImage, binaryImage, highThresh, 255, CV_THRESH_BINARY_INV);

	//	imshow("binary", binaryImage);
	//	imwrite("44.jpg", binaryImage);

	//step3 morphology open
	Mat element3(3, 3, CV_8U, cv::Scalar(1));
	morphologyEx(binaryImage, morphologyImage, MORPH_OPEN, element3);

	imwrite("33.jpg", morphologyImage);

	//step4 findContour
	vector<vector<Point> > contours;
	Mat contourOutput = morphologyImage.clone();
	findContours(contourOutput, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//step4' Draw the contours
	cv::Mat contourImage(morphologyImage.size(), CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Scalar colors = Scalar(0, 0, 255);
	for (size_t idx = 0; idx < contours.size(); idx++)
	{
		cv::drawContours(contourImage, contours, idx, colors);
	}
	//	imshow("contour", contourImage);

	//step5 find the max contour
	double maxArea = 0;
	double sumArea = 0;
	vector<cv::Point> maxContour;

	for (size_t idx = 0; idx < contours.size(); idx++)
	{
		double area = cv::contourArea(contours[idx]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[idx];
		}
		sumArea += area;
	}

	//step6 将轮廓转为矩形框,计算轮廓面积和矩形面积的比值contourAreaRatio
	float contourAreaRatio = 0.0;
	float maxAreaRatio = 0.0;
	cv::Rect maxRect;
	if (maxArea > 0)
	{
		maxRect = cv::boundingRect(maxContour);
		//cv::Rect maxRect = myBoundingRect(maxContour);
		contourAreaRatio = (float)maxArea / maxRect.area();
		maxAreaRatio = (float)(maxArea / sumArea);
	}

	if (contourAreaRatio > 0.93 && maxAreaRatio > 0.75 && flag)
	{
		unsigned int backgroundMean = 0;
		float backgroundSigma = 0.0;
		//process image step1 get background gray
		histoManagementInstance.ostuHistogramWithoutWhiteBoarder(hist2, highThresh, backgroundMean, backgroundSigma);

		//process image step2 filling with background gray
		filling(dstImage, maxRect, backgroundMean, backgroundSigma);

		myMedianBlur(dstImage, maxRect, backgroundMean);

		stringstream ss;
		ss << "backgroundMean = " << backgroundMean << "\t" << "backgroundSigma = " << backgroundSigma;
		string str = imageDstFile + ss.str();

		LogManagement logManagementInstance;
		logManagementInstance.recordLog(LOG_IMAGE_PROCESS_PARAM, str);

		Mat cimg = Mat::eye(dstImage.cols, dstImage.rows, CV_8UC3);
		cvtColor(dstImage, cimg, CV_GRAY2BGR);

		FolderFileManagement folderFileManagementInstance;
		folderFileManagementInstance.createFolderForFile(imageDstFile);
		imwrite(imageDstFile, cimg);

		/*		//输出，以便查看正确性
		stringstream ss1,ss2;
		ss2 <<"Z:\\output\\"<<outputFileIndex<<".jpg";
		ss1 << "Z:\\input\\" << outputFileIndex << ".jpg";

		createFolderForFile(ss1.str());
		createFolderForFile(ss2.str());
		imwrite(ss1.str(), srcImage);
		imwrite(ss2.str(), cimg);
		outputFileIndex++;
		*/
		//把文件名写入file
		logManagementInstance.recordLog(LOG_PADDING_FILE, (string)imageSrcFile);

		return true;
	}
	else
	{
		//暂时不能处理
		//		createFolderForFile(imageDstFile);
		//		imwrite(imageDstFile, srcImage);
		return false;
	}
}

void ImageProcessManagement::pixelFill(Mat& dstImage, int row, int col, unsigned int  mean, float sigma)
{
	int sigmaInt = (int)sigma;
	int fillValue = mean + (rand() % sigmaInt - sigmaInt / 2);
	if (fillValue > 255)
	{
		fillValue = 255;//log
	}
	else if (fillValue < 0)
	{
		fillValue = 0;//log
	}
	dstImage.at<uchar>(row, col) = (unsigned char)fillValue;
}

void ImageProcessManagement::pixelFillSpecial(Mat& dstImage, int row, int col, unsigned int  mean, float sigma)
{
	int sigmaInt = (int)sigma;
	int fillValue = mean + (rand() % sigmaInt - sigmaInt / 2);
	if (fillValue > 255)
	{
		fillValue = 255;//log
	}
	else if (fillValue < 0)
	{
		fillValue = 0;//log
	}
	if (dstImage.at<uchar>(row, col) > 245)
		dstImage.at<uchar>(row, col) = (unsigned char)fillValue;
}
void ImageProcessManagement::filling(Mat& dstImage, cv::Rect maxRect, unsigned int  mean, float sigma)
{
	if (sigma > 2.0)
	{
		//filling row
		for (int row = 0; row < maxRect.y; row++)
		{
			for (int col = 0; col < 64; col++)
			{
				pixelFill(dstImage, row, col, mean, sigma);
			}
		}

		for (int row = maxRect.y + maxRect.height; row < 64; row++)
		{
			for (int col = 0; col < 64; col++)
			{
				pixelFill(dstImage, row, col, mean, sigma);
			}
		}

		//filling col
		for (int col = 0; col < maxRect.x; col++)
		{
			for (int row = 0; row < 64; row++)
			{
				pixelFill(dstImage, row, col, mean, sigma);
			}
		}

		for (int col = maxRect.x + maxRect.width; col < 64; col++)
		{
			for (int row = 0; row < 64; row++)
			{
				pixelFill(dstImage, row, col, mean, sigma);
			}
		}
	}
	else
	{
		//filling row
		for (int row = 0; row < maxRect.y; row++)
		{
			for (int col = 0; col < 64; col++)
			{
				dstImage.at<uchar>(row, col) = (unsigned char)mean;
			}
		}

		for (int row = maxRect.y + maxRect.height; row < 64; row++)
		{
			for (int col = 0; col < 64; col++)
			{
				dstImage.at<uchar>(row, col) = (unsigned char)mean;
			}
		}

		//filling col
		for (int col = 0; col < maxRect.x; col++)
		{
			for (int row = 0; row < 64; row++)
			{
				dstImage.at<uchar>(row, col) = (unsigned char)mean;
			}
		}

		for (int col = maxRect.x + maxRect.width; col < 64; col++)
		{
			for (int row = 0; row < 64; row++)
			{
				dstImage.at<uchar>(row, col) = (unsigned char)mean;
			}
		}
	}


	//最下面一行特殊处理
	if (sigma > 2.0)
	{
		//filling row
		for (int row = 0; row < 64; row++)
		{
			if (row == maxRect.y || row == maxRect.y + 1 ||
				row == maxRect.y + maxRect.height - 1 || row == maxRect.y + maxRect.height - 2)
			{
				for (int col = 0; col < 64; col++)
				{
					pixelFillSpecial(dstImage, row, col, mean, sigma);
				}
			}
		}

		for (int col = 0; col < 64; col++)
		{
			if (col == maxRect.x || col == maxRect.x + 1 ||
				col == maxRect.x + maxRect.width - 1 || col == maxRect.x + maxRect.width - 2)
			{
				for (int row = 0; row < 64; row++)
				{
					pixelFillSpecial(dstImage, row, col, mean, sigma);
				}
			}
		}
	}
	else
	{
		//filling row

	}

}

void ImageProcessManagement::myMedianBlur(Mat dstImage, cv::Rect maxRect, const unsigned char backgroundMean)
{
	//top two row
	for (int row = 1; row < 63; row++)
	{
		for (int col = 1; col < 63; col++)
		{
			if ((row == maxRect.y || row == maxRect.y + 1 ||
				row == maxRect.y + maxRect.width - 1 || row == maxRect.y + maxRect.width - 2) ||
				(col == maxRect.x || col == maxRect.x + 1 ||
				col == maxRect.x + maxRect.width - 1 || col == maxRect.x + maxRect.width - 2))
			{
				if (dstImage.at<uchar>(row, col) > backgroundMean)
				{
					vector<unsigned char> pixels;
					pixels.push_back(dstImage.at<uchar>(row, col - 1));
					pixels.push_back(dstImage.at<uchar>(row, col + 1));
					pixels.push_back(dstImage.at<uchar>(row, col));
					pixels.push_back(dstImage.at<uchar>(row - 1, col - 1));
					pixels.push_back(dstImage.at<uchar>(row - 1, col + 1));
					pixels.push_back(dstImage.at<uchar>(row - 1, col));
					pixels.push_back(dstImage.at<uchar>(row + 1, col - 1));
					pixels.push_back(dstImage.at<uchar>(row + 1, col + 1));
					pixels.push_back(dstImage.at<uchar>(row + 1, col));

					sort(pixels.begin(), pixels.end());

					dstImage.at<uchar>(row, col) = pixels.at(5);
				}
			}
		}
	}
}