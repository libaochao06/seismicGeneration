#include "seisGenUtils.h"

void narrowBandAdjust(std::vector<double> &acc, const Spectrum &targetRsp, SeisGenPara params, std::ofstream &logFile)
{
    double dt, damp;//时间增量，阻尼比
    dt=params.dt;
    damp=targetRsp.getDamp();
    int nSize=acc.size();//人工时程长度
    std::vector<double> freqCtrl=targetRsp.getXSeries();//频率控制点数组
    double deltaS;//反应谱差值
    std::vector<double> deltaAcc;//增量时程
    for(int i=0;i<freqCtrl.size();i++)
    {
        //
        deltaAcc.clear();
        std::vector<double> temp;
        double freq, maxAcc, maxTime, omega, omegac;//频率，最大加速度， 最大加速度出现时刻，角频率，角频率带宽
        freq=freqCtrl.at(i);
        omega=PI2*freq;
        //求角频率带宽
        if(i==0)
        {
            omegac=0.5*(freqCtrl[i+1]-freq)*PI2;
        }
        else if(i==freqCtrl.size()-1)
        {
            omegac=0.5*(freq-freqCtrl[i-1])*PI2;
        }
        else
        {
            omegac=fmin(freq-freqCtrl[i-1], freqCtrl[i+1]-freq);
            omegac=omegac*0.5*PI2;
        }
        //***************
        //求最大响应及其出现时刻
        maxResp(acc, freq, damp, dt, temp);
        maxAcc=temp[0];
        maxTime=temp[1];
        //求反应谱差值
        if(maxAcc>0)
        {
            deltaS=targetRsp[i].getY()-fabs(maxAcc);
        }
        else if(maxAcc=0)
        {
            deltaS=0;
        }
        else
        {
            deltaS=-1*(targetRsp[i].getY()-fabs(maxAcc));
        }
        //构造响应窄带时程
        for(int j=0;j<nSize;j++)
        {
            double t, value;
            t=j*dt;
            value=deltaS*(sin(omegac*(t-maxTime)))/omegac/(t-maxTime)*cos(omega*(t-maxTime));
            deltaAcc.push_back(value);
        }
        //反演计算地震窄带时程
        double deltaFreq;
        deltaFreq=1.0/(nSize-1)/dt;
        vector<double> Haw;//时程传递函数

    }
}