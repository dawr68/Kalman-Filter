#pragma once
#include <string>
#include <Windows.h>

class FilterManager
{
	bool libraryFlag = 0; //0-asm 1-cpp

	std::string dataFilePath;
	
	int dataSize = 0;

	float *rawGyroData[3];
	float *rawAccData[3];

	float *rawAngles[3]; //X Y Z
	float *filteredAngles[3]; //X Y Z

	int numberOfThreads = 1;
	float lastExeDuration = 0;

public:
	FilterManager();
	~FilterManager();

	void readRawDataFromFile(std::string dataFile);

	void calculateAngles();

	void execute();
	
	void setDataFilePath(std::string path);
	std::string getDataFilePath();

	void setNumberOfThreads(int num);
	int getNumberOfThreads();
	int getDataSize();

	float** getRawGyroData();
	float** getRawAccData();

	float** getRawAngles();
	float** getFilteredAngles();

	float getLastExeDur();
	bool getLastUsedLibrary();

	void chooseAsmLibrary();
	void chooseCppLibrary();
};

