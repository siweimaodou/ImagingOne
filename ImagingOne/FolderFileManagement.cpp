#include "FolderFileManagement.h"
#include <iostream>
#include <direct.h> 
#include <io.h>  

#include "LogManagement.h"
using namespace std;


FolderFileManagement::FolderFileManagement()
{
}


FolderFileManagement::~FolderFileManagement()
{
}


void FolderFileManagement::getFiles(string path, vector<string>& files, string postfix)
{
	LogManagement logManagementInstance;
	cout << "enter getFilse" << endl;
	//�ļ����      
	intptr_t   hFile = 0;
	//�ļ���Ϣ      
	struct _finddata_t fileinfo;

	string pathName, exdName = "\\*";
	string dirPath = path;

	if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮      
			//�������,�����б�      
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(pathName.assign(path).append("\\").append(fileinfo.name), files, postfix);
					dirPath.append("\\").append(fileinfo.name);
				}
			}
			else
			{
				if (string(fileinfo.name).find(postfix) != string::npos)
				{
					string filepath = path + "\\" + string(fileinfo.name);
					files.push_back(filepath);
					logManagementInstance.recordLog(LOG_FOR_FILE, filepath);
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}



void FolderFileManagement::createFolderForFile(const string& fullFileName)
{
	const char *tag;
	const char *fileName = fullFileName.c_str();
	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			char buf[1000], path[1000];
			strcpy(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy(path, buf);
			if (_access(path, 6) == -1)
			{
				_mkdir(path);  //�����ɹ�����0 ���ɹ�����-1  
			}
		}
	}
}