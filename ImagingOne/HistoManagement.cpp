#include "HistoManagement.h"


HistoManagement::HistoManagement()
{
}


HistoManagement::~HistoManagement()
{
}


unsigned char HistoManagement::getTopThresh(MatND histo)
{
	MatND tmpHisto = histo.clone();
	unsigned char thresh = 210;
	for (int i = 3; i < 252; i++)
	{
		tmpHisto.at<float>(i) = (histo.at<float>(i - 3) + histo.at<float>(i - 2) + histo.at<float>(i - 1) +
			histo.at<float>(i) +histo.at<float>(i + 1) + histo.at<float>(i + 2) + histo.at<float>(i + 3)) / 7;
	}
	for (int i = 252; i > 210; i--)
	{
		if (tmpHisto.at<float>(i) < 2.0)
		{
			thresh = i;
			break;
		}
	}
	return thresh;
}

bool HistoManagement::histoIncludeTwoPeak(MatND& histo, unsigned char highThresh)
{
	const float EPSINON = (float)0.000001;
	MatND tmpHisto = histo.clone();
	for (int i = 3; i < 252; i++)
	{
		tmpHisto.at<float>(i) = (histo.at<float>(i - 3) + histo.at<float>(i - 2) + histo.at<float>(i - 1) +
			histo.at<float>(i) +histo.at<float>(i + 1) + histo.at<float>(i + 2) + histo.at<float>(i + 3)) / 7;
	}

	float diffHisto[255];
	for (int i = 0; i < 254; i++)
	{
		diffHisto[i] = tmpHisto.at<float>(i + 1) - tmpHisto.at<float>(i);
	}
	int peakNum = 0;
	int peakIndex[255];
	float maxHisto = 0.0;
	for (int i = 3; i < highThresh; i++)
	{
		if (tmpHisto.at<float>(i) > maxHisto)
		{
			maxHisto = tmpHisto.at<float>(i);
		}
	}
	float tmpThresh = (float)(maxHisto / 10.0);
	if (tmpThresh < 5)
	{
		tmpThresh = 5;
	}
	for (int i = 3; i < highThresh; i++)
	{
		if ((tmpHisto.at<float>(i) > tmpThresh) && (diffHisto[i] > EPSINON) && (diffHisto[i + 1] < EPSINON))
		{
			//			 && 
			peakIndex[peakNum++] = i;
		}
	}

	if (peakNum < 2)
	{
		return false;
	}
	else
	{
		int maxIntervalOfPeak = peakIndex[peakNum - 1] - peakIndex[0];
		if (maxIntervalOfPeak < 40)
		{
			return false;
		}
	}
	return true;
}


void HistoManagement::ostuHistogramWithoutWhiteBoarder(MatND histo, unsigned char maxThresh, unsigned int& backgroundMean, float& backgroundSigma)
{
	//ostu threshold
	int MAX_THRESH = maxThresh;

	float sum = 0.0;
	float mean = 0.0;

	long sum0 = 0, sum1 = 0; //存储前景的灰度总和和背景灰度总和  
	long cnt0 = 0, cnt1 = 0; //前景的总个数和背景的总个数  
	double w0 = 0, w1 = 0; //前景和背景所占整幅图像的比例  
	double u0 = 0, u1 = 0;  //前景和背景的平均灰度  
	double variance = 0; //最大类间方差  
	double u = 0;
	double maxVariance = 0;
	long size = 0;
	int threshold;
	int sigma1;

	//两次ostu,第二次开始点为第一次的输出结果。
	int initThresh[2] = { 1, 1 };
	for (int k = 0; k < 2; k++)
	{
		size = 0;
		sum = 0;
		for (int i = initThresh[k]; i < MAX_THRESH; i++)
		{
			size += (long)histo.at<float>(i);
			sum += i * histo.at<float>(i);
		}
		if (size == 0)
		{
			//log exception
			return;
		}
		mean = sum / size;

		maxVariance = 0;
		for (int i = initThresh[k]; i < MAX_THRESH; i++)
		{
			sum0 = 0;
			sum1 = 0;
			cnt0 = 0;
			cnt1 = 0;
			w0 = 0;
			w1 = 0;

			for (int j = initThresh[k]; j < i; j++)
			{
				cnt0 += (long)histo.at<float>(j);
				sum0 += (long)(j * histo.at<float>(j));
			}
			if (cnt0 > 0)
			{
				u0 = (double)sum0 / cnt0;
				w0 = (double)cnt0 / size;
			}

			for (int j = i; j < MAX_THRESH; j++)
			{
				cnt1 += (long)histo.at<float>(j);
				sum1 += (long)(j * histo.at<float>(j));
			}
			if (cnt1 > 0)
			{
				u1 = (double)sum1 / cnt1;
				w1 = (double)cnt1 / size;

				long squareSum = 0;
				for (int j = i; j < MAX_THRESH; j++)
				{
					squareSum += (long)pow((histo.at<float>(j) -u1), 2);
				}
				sigma1 = (int)sqrt(squareSum / cnt1);
			}

			u = u0 * w0 + u1 * w1; //图像的平均灰度 
			variance = w0 * w1 *  (u0 - u1) * (u0 - u1);
			if (variance > maxVariance)
			{
				maxVariance = variance;
				threshold = i;
				backgroundMean = (unsigned int)u1;
				backgroundSigma = (float)sigma1;
			}
		}
		if (backgroundMean - backgroundSigma / 2 > mean)
			initThresh[1] = (int)(backgroundMean - backgroundSigma / 2);
		else
			initThresh[1] = (int)mean;

	}
}

//参数：Gray_img --输入的灰度图像  
//      hist 计算后输出的直方图  
void HistoManagement::myCal_Hist(Mat Gray_img, MatND& hist)
{

	int bins = 256;
	int hist_size = bins;
	float range[] = { 0, 256 };
	const float* ranges = { range };

	//计算直方图  
	calcHist(&Gray_img, 1, 0, Mat(), // do not use mask    
		hist, 1, &hist_size, &ranges,
		true, // the histogram is uniform    
		false);

	//绘制直方图图像  
	int hist_height = 256;
	//int bins = 256;  
	double max_val;  //直方图的最大值  
	int scale = 2;   //直方图的宽度  
	minMaxLoc(hist, 0, &max_val, 0, 0); //计算直方图最大值  

	Mat hist_img = Mat::zeros(hist_height, bins*scale, CV_8UC3); //创建一个直方图图像并初始化为0   

	//在直方图图像中写入直方图数据  
	for (int i = 0; i<bins; i++)
	{
		float bin_val = hist.at<float>(i); // 第i灰度级上的数      
		int intensity = cvRound(bin_val*hist_height / max_val);  //要绘制的高度    
		//填充第i灰度级的数据  
		rectangle(hist_img, Point(i*scale, hist_height - 1),
			Point((i + 1)*scale - 1, hist_height - intensity),
			CV_RGB(255, 255, 255));
	}

	//	imshow("Gray Histogram2", hist_img);
}
