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
bool rspToPsdVanmarckeIterate(const Spectrum &tRsp, powerSpectrumDensity &psd, double deltaFreq, double Td, int N, std::ofstream &logFile);
//峰值系数计算函数
double rPeak(double probability, double Td, double freq, double damp);
//人工时程包络曲线计算函数
void envelopeFuncCal(const double Td, const double tRise, const double tDrop, const double dt, const double nFour, std::vector<double> &envFunc);
//人工时程包络曲线包络函数
void accEnvelop(std::vector<double> &acc, const std::vector<double> &envFunc);
//相位角计算函数
void phaseAngleFuncCal(const SeisGenPara &params, int nFour, std::vector<double> &phaseAngleFunc, phaseAngleCalMethod method);
//初始时程计算函数
void initAcc(const powerSpectrumDensity &psd, const std::vector<double> &phaseAngle, const int nFour, std::vector<double> &accTimeHist);
//快速傅里叶变换计算函数
void fastFourierTrans(const std::vector<std::complex<double>> &in, std::vector<std::complex<double>> &out, int inv);
//峰值调整计算函数
void peakAdjust(std::vector<double> &acc, double amp);
//时程曲线削峰函数
void peakReduction(std::vector<double> &acc, double amp, const std::vector<double> &envFunc);
void peakReduction(std::vector<double> &acc, double amp);
//基线调整计算函数
void baselineAdjust(std::vector<double> &acc, double dt);
//加速度积分求速度和位移曲线
void accTimeHistIntegral(const std::vector<double> &acc, std::vector<double> &vel, std::vector<double> &disp, double dt);
//时程曲线积分求反应谱函数
void timeHistToSpectrum(const std::vector<double> &acc, const std::vector<double> &freqCtrl, double dt, Spectrum &calSpec);
//纽马克线性加速度法积分
void integralNewmark(const std::vector<double> &acc, const std::vector<double> &freqCtrl, double dt, Spectrum &calSpec);
//计算反应谱与目标谱反应包络性检查
bool targetRspEnvCheck(const Spectrum &targetRsp, const Spectrum &calRsp, std::ofstream &logFile);
bool targetRspEnvCheck(const rspError &err, std::ofstream &logFile);
//根据目标反应谱对人工时程进行调整
void fourierAmplitudeAdjust(std::vector<double> &acc, const Spectrum &targetRsp, const SeisGenPara &params, const std::vector<double> &phaseAngle, std::ofstream &logFile);
//最大反应计算
void maxResp(const std::vector<double> &acc, double freq, double damp, double dt, std::vector<double> &results);
double maxResp(const std::vector<double> &acc, double freq, double damp, double dt);
//窄带时程调整法对人工时程进行调整
void narrowBandAdjust(std::vector<double> &acc, const Spectrum &targetRsp, const SeisGenPara &params, std::ofstream &logFile);
//人工时程拟合调整函数
void timeHistAdjust(std::vector<double> &acc, const Spectrum &targetRsp, const SeisGenPara &params, const std::vector<double> &phaseAngle, std::ofstream &logFile);
//计算反应谱与目标反应谱误差计算函数
rspError errorCalRspToTargetRsp(const Spectrum &targetRsp, const Spectrum &calRsp);
//根据目标功率谱进行人工时程调整
void targetPsdAdjust(std::vector<double> &acc, const powerSpectrumDensity &psd, const SeisGenPara &params, const std::vector<double> &phaseAngle, std::ofstream &logFile);
//由人工时程计算功率谱函数
void timeHistToPsd(const std::vector<double> &acc, powerSpectrumDensity &psd, const SeisGenPara &params);
//查找时程曲线绝对值的最大值
double maxAbsOfTimeHist(const std::vector<double> &acc);
//时程响应卷积计算方法
void respConvolution(const std::vector<double> &f, const std::vector<double> &g, std::vector<double> &out, const SeisGenPara &params);
//时程曲线整体缩放
//**按照比例系数及曲线进行缩放
void timeHistScale(std::vector<double> &acc, const double factor, const std::vector<double> &curve);
//**按照比例系数进行缩放
void timeHistScale(std::vector<double> &acc, const double factor);
//计算传递函数
void transFunction(double omega0, double deltaOmega, double damp, int nSize, std::vector<std::complex<double>> &Hw);
//窄带时程构造函数
void narrowBandAcc(double deltaS, double omegac, double omega, double maxTime, int nSize, double dt, std::vector<double> &deltaAcc);
//窄带时程反演计算函数
void nBandAccRevByTransFunc(std::vector<double> &deltaAcc, const std::vector<std::complex<double>> &Haw);
//窄带时程端头振荡调节函数
void nBandAccEndAdjust(std::vector<double> &deltaAcc, const double damp, double dt, const double deltaS, const double maxTime, const double omegac, const double omega);
#endif