#include "seisGenUtils.h"

void integralNewmark(const std::vector<double> &acc, const std::vector<double> &freqCtrl, double dt, Spectrum &calSpec)
{
    //阻尼比
    double damp=calSpec.getDamp();
    int numFreq=freqCtrl.size();//频率控制点个数
    calSpec.getData().clear();//清空反应谱数据
    for(int i=0;i<numFreq;i++)
    {
        double freq, omega;
        freq=freqCtrl.at(i);
        omega=PI2*freq;
        double ui, vi, uiNew, viNew, ag;
        double accOld, accNew;
        int nSize=acc.size();
        ui=0;
        vi=0;
        double maxAcc;
        //accOld=fabs(2*damp*omega*acc.front()*dt);
        accOld=acc.front();
        maxAcc=fabs(accOld);
        double a1, a2, a3, a4;
        for(int j=1;j<nSize;j++)
        {
            ag=acc.at(j);
            a1=1+damp*omega*dt+pow(omega*dt,2)/4;
            a2=damp*omega*dt+pow(omega*dt,2)/4;
            a3=2*damp*omega+pow(omega,2)*dt;
            a4=pow(omega,2);
            accNew=(-ag-a2*accOld-a3*vi-a4*ui)/a1;
            viNew=vi+dt/2*(accOld+accNew);
            uiNew=ui+vi*dt+pow(dt,2)/2*(accOld+accNew)/2;
            maxAcc=fmax(maxAcc,fabs(accNew+ag));
            //
            accOld=accNew;
            ui=uiNew;
            vi=viNew;
        }
        calSpec.addData(DataPoint(freq,maxAcc));
    }
}