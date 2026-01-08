/*
* CryptoGarage - GaussCurve
* 
* TODO
*/

#pragma once

#include <Arduino.h>
#include <math.h>
#include <PrintDebug.h>

class GaussCurve {
  public:
    GaussCurve(int min_output = 0, int max_output = 1023);
    bool generatePolynomFromPoints(double points[][2], int numberpoints);
    bool solve(double * mat, double * solution, int size);
    double plot(double * parameters, int size, double x);
    double calc(double x);
  
  private:
    bool gaussianElimination(double * mat, int size, double * solution);
    double parameters[10];
    int size = 0;
    int min = 0;
    int max = 0;
};
