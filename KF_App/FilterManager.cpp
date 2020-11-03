#include "FilterManager.h"
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

typedef int(_fastcall* filterProc)(float* rawGyroData, float* rawAccAngle, float* filteredData, int dataSize);

FilterManager::FilterManager()
{
    for (auto& axis : rawGyroData)
        axis = 0;

    for (auto& axis : rawAccData)
        axis = 0;

    for (auto& axis : rawAngles)
        axis = 0;

    for (auto& axis : filteredAngles)
        axis = 0;
}


FilterManager::~FilterManager()
{
    for (auto &axis : rawGyroData)
        delete[] axis;

    for (auto &axis : rawAccData)
        delete[] axis;

    for (auto &axis : rawAngles)
        delete[] axis;

    for (auto &axis : filteredAngles)
        delete[] axis;
}


void FilterManager::readRawDataFromFile(std::string dataFile)
{
    dataSize = 341;

    for (auto& axis : rawGyroData)
        axis = new float[dataSize];

    for (auto& axis : rawAccData)
        axis = new float[dataSize];

    for (auto& axis : rawAngles)
        axis = new float[dataSize];

    for (auto& axis : filteredAngles)
        axis = new float[dataSize];

    std::ifstream inputFile;
    inputFile.open(dataFile);

    if (inputFile.is_open())
    {
        std::string line;
        int value;

        if (std::getline(inputFile, line)) //read header
        {
            int rowIdx = 0;
            while (std::getline(inputFile, line) && rowIdx < dataSize)
            {
                std::stringstream ss(line);

                for (int i = 0; i < 3; i++)
                    if (ss >> value)
                        rawGyroData[i][rowIdx] = value * 250. / 32768.;

                for (int i = 0; i < 3; i++)
                    if (ss >> value)
                        rawAccData[i][rowIdx] = value * 4. / 65535.;

                rowIdx++;
            }
        }

        inputFile.close();
    }

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < dataSize; j++)
            filteredAngles[i][j] = 0;

    calculateAngles();
}

void FilterManager::calculateAngles()
{
    for (int i = 0; i < dataSize; i++)
    {
        rawAngles[0][i] = atan(rawAccData[1][i] / rawAccData[2][i]) * 180. / M_PI; //X
        rawAngles[1][i] = atan(rawAccData[0][i] / rawAccData[2][i]) * 180. / M_PI; //Y
        rawAngles[2][i] = atan(rawAccData[0][i] / rawAccData[1][i]) * 180. / M_PI; //Z
    }
}


void FilterManager::execute()
{
    HINSTANCE hLib = NULL;

    if(libraryFlag)
        hLib = LoadLibrary(L"KF_Cpp.dll");
    else
        hLib = LoadLibrary(L"KF_Asm.dll");

    if (hLib == NULL)
        return;


    filterProc filter = (filterProc)GetProcAddress(hLib, "filter");

    if (!filter)
        return;

    for (int i = 0; i < 3; i++)
    {
        if (rawGyroData[i] == 0 || rawAccData[i] == 0)
            return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 3; i++)
    {
        filter(rawGyroData[i], rawAngles[i], filteredAngles[i], dataSize);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    lastExeDuration = duration.count();


    FreeLibrary(hLib);
}


void FilterManager::setDataFilePath(std::string path)
{
    dataFilePath = path;
}


std::string FilterManager::getDataFilePath()
{
    return dataFilePath;
}


void FilterManager::setNumberOfThreads(int num)
{
    if (num >= 1 || num <= 6)
        numberOfThreads;
}


int FilterManager::getNumberOfThreads()
{
    return numberOfThreads;
}


int FilterManager::getDataSize()
{
    return dataSize;
}


float** FilterManager::getRawGyroData()
{
    return rawGyroData;
}

float** FilterManager::getRawAccData()
{
    return rawAccData;
}


float** FilterManager::getRawAngles()
{
    return rawAngles;
}



float** FilterManager::getFilteredAngles()
{
    return filteredAngles;
}


float FilterManager::getLastExeDur()
{
    return lastExeDuration;
}


bool FilterManager::getLastUsedLibrary()
{
    return libraryFlag;
}


void FilterManager::chooseAsmLibrary()
{
    libraryFlag = 0;
}


void FilterManager::chooseCppLibrary()
{
    libraryFlag = 1;
}