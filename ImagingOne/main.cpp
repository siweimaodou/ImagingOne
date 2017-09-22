
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

#include "ImageProcessManagement.h"
#include "FolderFileManagement.h"
#include "LogManagement.h"
using namespace std;
using namespace cv;


const string LOG_PADDING_RECORD = "logPaddingRecord.txt";
const string LOG_NONE_PADDING_RECORD = "logNonePaddingRecord.txt";
const string LOG_IMAGE_PROCESS_PARAM = "imageProcessParam.txt";
const string LOG_PADDING_FILE = "paddingFiles.txt";

string tmpFileName = "Z:\\09";
		/*
		stringstream ss;
		ss << outputFileIndex++ <<".jpg";
		output = output + "\\" + ss.str();

		tmpFileName.clear();
		tmpFileName.append("output2");
		tmpFileName = tmpFileName + "\\" + ss.str();
		*/
int index = 0;
int outputFileIndex = 0;
void main()
{
	string fileFolder[6] = { "D:\\00", "Z:\\00", "Z:\\01", "Z:\\02", "Z:\\03", "Z:\\04" };

	ImageProcessManagement imageProcessManagementInstance;
	FolderFileManagement folderFileManagementInstance;
	LogManagement logManagementInstance;


	double time0 = static_cast<double>(getTickCount());
	for (int i = 0; i < 1; i++)
	{
		//step 1: read out *.jpg from folder
		vector<string>filePath;
		folderFileManagementInstance.getFiles(fileFolder[i], filePath, ".jpg");
		
		//step 2: 遍历每个文件，对图像进行处理，存储处理后的文件	
		for (vector<string>::const_iterator iter = filePath.cbegin(); iter != filePath.cend(); iter++)
		{
			cout << "total 99000, current is processing : " << index++ << endl;
			//		string output = "output";
			//		output = output + "\\" + (*iter);

			string output = (*iter);
			output.replace(output.find(fileFolder[i]), 5, "D:\\08");

			if (imageProcessManagementInstance.paddingImage((*iter), output))
			{
				logManagementInstance.recordLog(LOG_PADDING_RECORD, output);
			}
			else
			{
				logManagementInstance.recordLog(LOG_NONE_PADDING_RECORD, output);
			}
		}
	}
	
	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	cout << "total processing time is : " << time0 << "seconds"<<endl;

	waitKey(0);
}











