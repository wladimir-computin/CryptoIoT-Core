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
    bool generateTableFromPoints(double points[][2], int numberpoints, int maximum, int * lookuptable, int size_lookup);
    bool solve(double * mat, double * solution, int size);
    double plot(double * parameters, int size, double x);
    void generateLookupTable(double * parameters, int maximum, int size, int * lookuptable, int size_lookup);
  
  private:
    bool gaussianElimination(double * mat, int size, double * solution);
};
