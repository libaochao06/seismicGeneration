#ifndef SEISGENUTILS_H_
#define SEISGENUTILS_H_

#include <string>
#include <fstream>
#include <iostream>
#include "seisGenPara.h"
#include <vector>
#include <sstream>
#include <cmath>
#include "spectrum.h"
#include "powerSpectrumDensity.h"
#include <complex>

void readInput(const std::string &infileName, SeisGenPara &params, std::vector<Spectrum> &specs, std::ofstream &logFile);
void split(const std::string& s, vector<std::string>& sv, const char flag = ' ');
void trim(std::string &str, const char flag=' ');
int str2int(const std::string &str);
double str2double(const std::string &str);
//变换反应谱数值单位，flag=1： g->m/s2; flag=0：m/s2->g
void changeSpectrumUnit(Spectrum &sp, int flag);
int FFTParams(double tdur, double dt, double &Td, double &deltaFreq);
//目标反应谱按照法规要求插值函数
int rspInterpolation(const Spectrum &oRsp, Spectrum &tRsp,std::ofstream &logFile);
//目标反应谱转功率密度谱函数1：Vanmarcke方法
bool rspToPsdVanmarcke(const Spectrum &tRsp, powerSpectrumDensity &psd, double deltaFreq, double Td, int N, std::ofstream &logFile);
//峰值系数计算函数
double rPeak(double probability, double Td, double freq, double damp);
//人工时程包络曲线计算函数
void envelopeFuncCal(const double Td, const double tRise, const double tDrop, const double dt, const double nFour, std::vector<double> &envFunc);
void phaseAngleFuncCal(const SeisGenPara &params, int nFour, std::vector<double> &phaseAngleFunc, phaseAngleCalMethod method);
void initAcc(const powerSpectrumDensity &psd, const std::vector<double> &phaseAngle, const int nFour, std::vector<double> &accTimeHist);
void fastFourierTrans(const std::vector<std::complex<double>> &in, std::vector<std::complex<double>> &out, int inv);
void peakAdjust(std::vector<double> &acc, double amp);
#endif