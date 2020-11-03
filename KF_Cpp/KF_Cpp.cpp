// KF_Cpp.cpp : Defines the exported functions for the DLL.
#include "KF_Cpp.h"

int filter(float* rawGyroData, float* rawAccAngle, float* filteredData, int dataSize)
{
    if (!rawGyroData || !rawAccAngle || !filteredData)
        return 1;

    float dt = 0.1f;

    float A[2][2] { {1, -dt}, {0 , 1} };
    float B[2] { dt, 0 };
    float C[2] { 1, 0 };

    float std_dev_v = 4;
    float std_dev_w = 3;

    float V[2][2]{ {std_dev_v * std_dev_v * dt, 0 }, {0, std_dev_v * std_dev_v * dt} };
    float W = std_dev_w * std_dev_w;

    float x0[2]{ 0, 0 };
    float P0[2][2] { {1,0}, {0,1} };
    float xpri[2]{ x0[0], x0[1]};
    float Ppri[2][2]{ {P0[0][0], P0[0][1]}, {P0[1][0], P0[1][1]} };
    float xpost[2]{ x0[0], x0[1] };
    float Ppost[2][2]{ {P0[0][0], P0[0][1]}, {P0[1][0], P0[1][1]} };

    xpost[0] = rawAccAngle[0];

    for (int i = 1; i < dataSize; i++)
    {
        float u = rawGyroData[i];

        //Aktualizacja czasu
        //xpri = A * xpost + B*u;
        xpri[0] = A[0][0] * xpost[0] + A[0][1] * xpost[1] + B[0] * u;
        xpri[1] = A[1][0] * xpost[0] + A[1][1] * xpost[1] + B[1] * u;
        //Ppri = A * Ppost * A' + V;
        Ppri[0][0] = Ppost[0][0] - Ppost[1][0]*dt + V[0][0];
        Ppri[0][1] = -Ppost[0][1]*dt + Ppost[0][1]*dt*dt + V[0][1]; 
        Ppri[1][0] = Ppost[1][0] - Ppost[1][1]*dt + V[1][0];
        Ppri[1][1] = Ppost[1][1] + V[1][1];


        //Aktualizacja pomiarow
        //eps = Y(i) - C*xpri;
        float eps = rawAccAngle[i] - C[0] * xpri[0];
        //float S = C * Ppri * C' + W;
        float S = Ppri[0][0] + W;
        //float K = Ppri * C' * (1 / S);
        float K[2];
        K[0] = Ppri[0][0] / S;
        K[1] = Ppri[1][0] / S;
        //xpost = xpri + K * eps;
        xpost[0] = xpri[0] + K[0] * eps;
        xpost[1] = xpri[1] + K[1] * eps;
        //Ppost = Ppri - K * S * K';
        Ppost[0][0] = Ppri[0][0] - S*K[0]*K[0];
        Ppost[0][1] = Ppri[0][1] - S*K[0]*K[1];
        Ppost[1][0] = Ppri[1][0] - S*K[1]*K[0];
        Ppost[1][1] = Ppri[1][1] - S*K[1]*K[1];

        filteredData[i] = xpost[0];
    }

    return 0;
}


