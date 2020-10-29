#pragma once
#include <string>
#include <Windows.h>

class FilterManager
{
	bool libraryFlag = 0; //0-asm 1-cpp

	std::string dataFilePath;
	
	int dataSize = 0;

	double *rawGyroData[3];
	double *rawAccData[3];

	double *filteredGyroData[3];
	double *filteredAccData[3];

	double numberOfThreads = 1;
	double lastExeDuration = 0;

public:
	FilterManager();
	~FilterManager();

	void readRawDataFromFile(std::string dataFile);

	void execute();
	
	void setDataFilePath(std::string path);
	std::string getDataFilePath();

	void setNumberOfThreads(int num);
	int getNumberOfThreads();
	int getDataSize();

	double** getRawGyroData();
	double** getFilteredGyroData();
	double** getRawAccData();
	double** getFilteredAccData();

	double getLastExeDur();
	bool getLastUsedLibrary();

	void chooseAsmLibrary();
	void chooseCppLibrary();
};

