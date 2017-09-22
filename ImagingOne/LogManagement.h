#pragma once
#include <string>
#include <fstream>
using namespace std;

class LogManagement
{
public:
	LogManagement();
	~LogManagement();

	void recordLog(const string& logFileName, string& logInfo);
	void logForFile(string& logFileName, string& logInfo);

	ofstream myOfsteam;
};

