#ifndef GLOBALDEFINES_H_
#define GLOBALDEFINES_H_

#include<array>
 
 //---------------Global constants------------------
extern const double PI;
extern const double PI2;
extern const double G;
extern const double EPS;

class freqCtrlStandard
{
//GB50267 表3.4.1中给出的建议反应谱的频率增量
public:
    static const std::array<double, 9> points;
    static const std::array<double, 8> increments;
};

class rspError
{
    public:
    double errSquareRoot;
    double maxNegErr;
    double maxPosErr;
    double maxErr;
    int nErr;
    int nBelow;
    int nAbove;
    rspError(double errSR, double mN, double mP, double mE, int nE, int nB, int nA);
    rspError(const rspError &err);
    rspError &operator=(const rspError & err);
};

enum class spectrumXType{Freq=1,Period=2, Omega=3};
enum class spectrumYType{Accel=0, Vel=1, Disp=2};
enum class PSDFlag{RG160=1, Vanmarcke=2, Kaul=3};
enum class phaseAngleCalMethod{Random=1, EnvFunc=2};

#endif