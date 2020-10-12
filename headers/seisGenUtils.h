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

void readInput(const std::string &infileName, SeisGenPara &params, std::vector<Spectrum> &specs, std::ofstream &logFile);
void split(const std::string& s, vector<std::string>& sv, const char flag = ' ');
void trim(std::string &str, const char flag=' ');
int str2int(const std::string &str);
double str2double(const std::string &str);
//变换反应谱数值单位，flag=1： g->m/s2; flag=0：m/s2->g
void changeSpectrumUnit(Spectrum &sp, int flag);
int FFTParams(double tdur, double dt, double &Td, double &deltaFreq);
int rspInterpolation(const Spectrum &oRsp, Spectrum &tRsp,std::ofstream &logFile);

#endif