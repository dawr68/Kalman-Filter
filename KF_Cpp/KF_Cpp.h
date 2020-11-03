#pragma once

#ifdef KFLIBRARY_EXPORTS
#define KFLIBRARY_API __declspec(dllexport)
#else
#define KFLIBRARY_API __declspec(dllimport)
#endif

extern "C" KFLIBRARY_API int filter(float* rawGyroData, float* rawAccAngle, float* filteredData, int dataSize);
