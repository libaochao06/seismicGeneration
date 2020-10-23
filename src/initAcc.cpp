#include "seisGenUtils.h"
#include <complex>

typedef std::complex<double> cp;

void initAcc(const powerSpectrumDensity &psd, const std::vector<double> &phaseAngle, const int nFour, std::vector<double> &accTimeHist)
{
    double dw;
    dw=(psd.data[1].getX()-psd.data[0].getX())*PI2;//等频率间隔近似处理
    //初始傅里叶幅值谱
    std::vector<double> ampFourier;
    for(auto it=psd.data.begin();it!=psd.data.end();it++)
    {
        double value;
        value=2.0*(it->getY())*dw;
        ampFourier.push_back(sqrt(value));
    }

    //傅里叶系数数组
    std::vector<cp> fourSeries(nFour);
    fourSeries[0]=cp(0,0);
    for(int i=1;i<nFour/2;i++)
    {
        double phi=phaseAngle.at(i-1);
        fourSeries[i]=ampFourier[i]*cp(cos(phi),sin(phi));
        fourSeries[nFour-i]=std::conj(fourSeries[i]);
    }
    fourSeries[nFour/2]=ampFourier[nFour/2-1];

    //由快速傅里中变换求初始时程的复数形式
    vector<cp> accTimeHistComplex;
    fastFourierTrans(fourSeries, accTimeHistComplex, 1);
    for(auto it=accTimeHistComplex.begin();it!=accTimeHistComplex.end();it++)
    {
        accTimeHist.push_back(it->real());
    }
}