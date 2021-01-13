#include "FilterManager.h"
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>

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
    std::ifstream testFile(dataFile);

    dataSize = std::count(std::istreambuf_iterator<char>(testFile),
        std::istreambuf_iterator<char>(), '\n');
    dataSize -= 1; //subtract header

    std::ifstream inputFile;
    inputFile.open(dataFile);

    for (auto& axis : rawGyroData)
        axis = new float[dataSize];

    for (auto& axis : rawAccData)
        axis = new float[dataSize];

    for (auto& axis : rawAngles)
        axis = new float[dataSize];

    for (auto& axis : filteredAngles)
        axis = new float[dataSize];

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

    if (numberOfThreads == 1) {
        std::thread t1(filter, rawGyroData[0], rawAngles[0], filteredAngles[0], dataSize);
        t1.join();
        std::thread t2(filter, rawGyroData[1], rawAngles[1], filteredAngles[1], dataSize);
        t2.join();
        std::thread t3(filter, rawGyroData[2], rawAngles[2], filteredAngles[2], dataSize);
        t3.join();
    }
    else
        if (numberOfThreads == 2) {
            std::thread t1(filter, rawGyroData[0], rawAngles[0], filteredAngles[0], dataSize);
            std::thread t2(filter, rawGyroData[1], rawAngles[1], filteredAngles[1], dataSize);
            t1.join();
            std::thread t3(filter, rawGyroData[2], rawAngles[2], filteredAngles[2], dataSize);
            t2.join();
            t3.join();
        }
        else {
            std::thread t1(filter, rawGyroData[0], rawAngles[0], filteredAngles[0], dataSize);
            std::thread t2(filter, rawGyroData[1], rawAngles[1], filteredAngles[1], dataSize);
            std::thread t3(filter, rawGyroData[2], rawAngles[2], filteredAngles[2], dataSize);
            t1.join();
            t2.join();
            t3.join();
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
        numberOfThreads = num;
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