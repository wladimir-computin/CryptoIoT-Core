/*
  CryptoGarage - GaussCurve

  (implementation)
*/

#include "GaussCurve_PWM.h"

bool GaussCurve::generateTableFromPoints(double points[][2], int numberpoints, int * lookuptable, int size_lookup) {
  double mat[numberpoints * numberpoints];
  double solution[numberpoints];

  for (int row = 0; row < numberpoints; row++) {
    for (int col = 0; col < numberpoints; col++) {
      mat[row * numberpoints + col] = pow(points[row][0], numberpoints - 1 - col);
    }
  }

  for (int row = 0; row < numberpoints; row++) {
    solution[row] = points[row][1];
  }

  if (solve(mat, solution, numberpoints)) {
    generateLookupTable(solution, numberpoints, lookuptable, size_lookup);
    String poly = String("f(x) = ") + String(solution[0], 10) + "x^" + (numberpoints - 1);
    for (int i = 1; i < numberpoints; i++) {
      if (solution[i] == 0) continue;
      if (solution[i] > 0) poly += " + ";
      poly += String(solution[i], 10) + "x^" + (numberpoints - i - 1);
    }
    poly.replace("-", " - ");
    poly.replace("x^0", "");
    printDebug(poly);
  } else {
    printDebug("Failed to generate polynom!");
	return false;
  }
  return true;
}

bool GaussCurve::solve(double * mat, double * solution, int size) {
  return gaussianElimination(mat, size, solution);
}

void GaussCurve::generateLookupTable(double * parameters, int size, int * lookuptable, int size_lookup) {
  for (int i = 0; i < size_lookup; i++) {
    lookuptable[i] = (int)round(plot(parameters, size, i));
  }
}

double GaussCurve::plot(double * parameters, int size, double x) {
  double out = 0;
  for (int i = 0; i < size; i++) {
    out += parameters[i] * pow(x, size - i - 1);
  }
  return out;
}

bool GaussCurve::gaussianElimination(double *A, int n, double *B) {
  int row, i, j, pivot_row;
  double max, dum, *pa, *pA, *A_pivot_row;

  // for each variable find pivot row and perform forward substitution

  pa = A;
  for (row = 0; row < (n - 1); row++, pa += n) {

    //  find the pivot row

    A_pivot_row = pa;
    max = fabs(*(pa + row));
    pA = pa + n;
    pivot_row = row;
    for (i = row + 1; i < n; pA += n, i++)
      if ((dum = fabs(*(pA + row))) > max) {
        max = dum; A_pivot_row = pA; pivot_row = i;
      }
    if (max == 0.0) return false;                // the matrix A is singular

    // and if it differs from the current row, interchange the two rows.

    if (pivot_row != row) {
      for (i = row; i < n; i++) {
        dum = *(pa + i);
        *(pa + i) = *(A_pivot_row + i);
        *(A_pivot_row + i) = dum;
      }
      dum = B[row];
      B[row] = B[pivot_row];
      B[pivot_row] = dum;
    }

    // Perform forward substitution

    for (i = row + 1; i < n; i++) {
      pA = A + i * n;
      dum = - *(pA + row) / *(pa + row);
      *(pA + row) = 0.0;
      for (j = row + 1; j < n; j++) *(pA + j) += dum * *(pa + j);
      B[i] += dum * B[row];
    }
  }

  // Perform backward substitution

  pa = A + (n - 1) * n;
  for (row = n - 1; row >= 0; pa -= n, row--) {
    if ( *(pa + row) == 0.0 ) return false;           // matrix is singular
    dum = 1.0 / *(pa + row);
    for ( i = row + 1; i < n; i++) *(pa + i) *= dum;
    B[row] *= dum;
    for ( i = 0, pA = A; i < row; pA += n, i++) {
      dum = *(pA + row);
      for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
      B[i] -= dum * B[row];
    }
  }
  return true;
}
