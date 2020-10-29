#include "FilterManager.h"
#include <chrono>

typedef int(_fastcall* filterProc)(const double* rawData, double* filteredData, const int dataSize);


FilterManager::FilterManager()
{
    for (auto& axis : rawGyroData)
        axis = 0;

    for (auto& axis : rawAccData)
        axis = 0;

    for (auto& axis : filteredGyroData)
        axis = 0;

    for (auto& axis : filteredAccData)
        axis = 0;
}


FilterManager::~FilterManager()
{
    for (auto &axis : rawGyroData)
        delete[] axis;

    for (auto &axis : rawAccData)
        delete[] axis;

    for (auto &axis : filteredGyroData)
        delete[] axis;

    for (auto &axis : filteredAccData)
        delete[] axis;
}


void FilterManager::readRawDataFromFile(std::string dataFile)
{
    dataSize = 1000;

    for (auto& axis : rawGyroData)
        axis = new double[dataSize];

    for (auto& axis : rawAccData)
        axis = new double[dataSize];

    for (auto& axis : filteredGyroData)
        axis = new double[dataSize];

    for (auto& axis : filteredAccData)
        axis = new double[dataSize];


    for(int i = 0; i < 3; i++)
        for (int j = 0; j < dataSize; j++)
        {
            if (i < (0.5 * dataSize))
            {
                rawGyroData[i][j] = 1. + (rand() % 200 - 100.) / 100.;
                rawAccData[i][j] = 10 + (rand() % 20 - 10.) / 10.;
            }
            else {
                rawGyroData[i][j] = 1.0 + (rand() % 200 - 100.) / 100. + 3.;
                rawAccData[i][j] = 10 + (rand() % 20 - 10.) / 10. - 7.;
            }
            
            filteredGyroData[i][j] = 0;
            filteredAccData[i][j] = 0;
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
        filter(rawGyroData[i], filteredGyroData[i], dataSize);
        filter(rawAccData[i], filteredAccData[i], dataSize);
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


double** FilterManager::getRawGyroData()
{
    return rawGyroData;
}


double** FilterManager::getFilteredGyroData()
{
    return filteredGyroData;
}


double** FilterManager::getRawAccData()
{
    return rawAccData;
}


double** FilterManager::getFilteredAccData()
{
    return filteredAccData;
}


double FilterManager::getLastExeDur()
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