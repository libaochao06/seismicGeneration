#include "globalDefines.h"

const double PI = 3.1415926;
const double PI2 = 2 * PI;
const double G = 9.81;
const double EPS = 1.0e-8;

const std::array<double, 9> freqCtrlStandard::points = { 0.2,3.0,3.6,5.0,8.0,15.0,18.0,22.0,33.0 };
const std::array<double, 8> freqCtrlStandard::increments = { 0.10,0.15,0.20,0.25,0.50,1.00,2.00,3.00 };