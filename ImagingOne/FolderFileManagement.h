#pragma once
#include <string>
#include <fstream>
#include <vector>
using namespace std;

class FolderFileManagement
{
public:
	FolderFileManagement();
	~FolderFileManagement();

	void getFiles(string path, vector<string>& files, string postfix);
	void createFolderForFile(const string& fullFileName);

	const string LOG_FOR_FILE = "logForFile.txt";
};

