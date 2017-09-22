#include "LogManagement.h"


LogManagement::LogManagement()
{
}


LogManagement::~LogManagement()
{
}

void LogManagement::recordLog(const string& logFileName, string& logInfo)
{
	myOfsteam.open(logFileName, ios::app);
	myOfsteam << logInfo.c_str() << endl;
	myOfsteam.close();
}

void LogManagement::logForFile(string& logFileName, string& logInfo)
{
	ofstream logFile(logFileName, ios_base::ate);

	logFile << logInfo << endl;

	logFile.close();
}