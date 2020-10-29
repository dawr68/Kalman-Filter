// KF_Cpp.cpp : Defines the exported functions for the DLL.
#include "KF_Cpp.h"

int filter(double* rawData, double* filteredData, int dataSize)
{
    if (!rawData || !filteredData)
        return 1;

    double dt = 0.1f;

    double std_dev = 10;

    double A = 1;
    double C = 1;

    double V = 10* std_dev * dt;
    double W = std_dev * std_dev;

    double x0 = 1;
    double P0 = 1;
    double xpri = x0;
    double Ppri = P0;
    double xpost = x0;
    double Ppost = P0;

    filteredData[0] = x0;

    for (int i = 1; i < dataSize; i++)
    {
        //Aktualizacja czasu
        xpri = A * xpost;
        Ppri = A * Ppost * A + V;

        //Aktualizacja pomiarow
        double eps = rawData[i] - C * xpri;
        double S = C * Ppri * C + W;
        double K = Ppri * C * (1 / S);
        xpost = xpri + K * eps;
        Ppost = Ppri - K * S * K;

        filteredData[i] = xpost;
    }

    return 0;
}