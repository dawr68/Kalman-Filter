/**
* Filtr Kalmana
* Algorytm implementuje filtracjê Kalmana na przekazanych danych z czujników IMU (akcelerometr i ¿yroskop).
* Semestr: 5 Rok: 3
* Autor: Dawid Rudy
* Wersja 1.0
*/
#include "KF_Cpp.h"

/* filter
* Prodecura wykonuje filtr kalmana na przekazanych danych znajduj¹cych siê w tablicach.
*   rawGyroData - wskaŸnik na tablicê zawieraj¹c¹ odczyty z ¿yroskopu
*   rawAccAngle - wskaŸnik na tablicê zawieraj¹c¹ k¹t pochylenia czujnika obliczony na podstawie odczytów z akcelerometru
*   filteredData - wskaŸnik na pust¹ tablicê w której zostan¹ umieszczone wyniki wykonania algorytmu
*   dataSize - wartoœæ oznaczaj¹ca rozmiar wszystkich przes³anych tablic
* Procedura zwraca 0 je¿eli filtracja zosta³a wykonana poprawnie.
*/
int filter(float* rawGyroData, float* rawAccAngle, float* filteredData, int dataSize)
{
    if (!rawGyroData || !rawAccAngle || !filteredData)
        return 1;

    float dt = 0.1f;

    float A[2][2] { {1, -dt}, {0 , 1} };
    float B[2] { dt, 0 };
    float H[2] { 1, 0 };

    //Wektory szumow procesowego i pomiarowego
    float stdDevV = 1;
    float stdDevW = 2;

    float V[2][2]{ {stdDevV * stdDevV * dt, 0 }, {0, stdDevV * stdDevV * dt} };
    float W = stdDevW * stdDevW;

    //ustawienie wartosci poczatkowych
    float xpri[2]{ 0, 0 };
    float Ppri[2][2]{ {1,0}, {0,1} };
    float xpost[2]{ 0, 0 };
    float Ppost[2][2]{ {1,0}, {0,1} };

    xpost[0] = rawAccAngle[0];

    for (int i = 1; i < dataSize; i++)
    {
        float u = rawGyroData[i];

        //Aktualizacja czasu
        float a = A[0][0] * xpost[0];
        float b = A[0][1] * xpost[1];
        float c = a + b;
        float d = u * B[0];
        float e = c + d;
        //xpri = A * xpost + B*u;
        xpri[0] = A[0][0] * xpost[0] + A[0][1] * xpost[1] + B[0]*u;
        xpri[1] = A[1][1] * xpost[1] + B[1]*u;
        //Ppri = A * Ppost * A' + V;
        Ppri[0][0] = Ppost[0][0] + Ppost[1][0]*A[0][1] + Ppost[0][1]*A[0][1] + Ppost[1][1]*A[0][1]*A[0][1] + V[0][0];
        Ppri[0][1] = Ppost[0][1] + Ppost[1][1]*A[0][1] + V[0][1]; 
        Ppri[1][0] = Ppost[1][0] + Ppost[1][1]*A[0][1] + V[1][0];
        Ppri[1][1] = Ppost[1][1] + V[1][1];

        //Aktualizacja pomiarow
        //eps = Y(i) - H*xpri;
        float eps = rawAccAngle[i] - xpri[0];
        //float S = H * Ppri * H' + W;
        float S = Ppri[0][0] + W;
        //K = Ppri * H' * (1 / S);
        float K[2]{ Ppri[0][0] / S, Ppri[1][0] / S };
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


