#include "seisGenUtils.h"
/*
 * @作者: Baochao LI
 * @版本: 1.0
 * @Date: 2020-04-16 18:52:08
 * @LastEditTime: 2020-04-17 19:31:47
 * @文件说明: 直接积分法，时程转反应谱实现文件
 */
void timeHistToSpectrum(const std::vector<double> &acc, const std::vector<double> &freqCtrl, double dt, Spectrum &calSpec)
{
    //阻尼比在定义反应谱时已经定义
    double damp=calSpec.getDamp();
    int numFreq=freqCtrl.size();//频率控制点个数
    calSpec.getData().clear();//清空反应谱数据
    //按频率控制数据进行遍历
    for(int i=0;i<numFreq;i++)
    {
        double freq, omega, omegaD;//频率，角频率，有阻尼角频率
        double den, wratio;//计算中间变量
        freq=freqCtrl.at(i);
        omega=PI2*freq;
        den=sqrt(1.0-pow(damp,2));
        omegaD=omega*den;//wd=w*sqrt(1-h^2)
        wratio=1.0/den;
        //积分公式中各常数中间量
        double hwdt, sinwdt, coswdt, etau;
        hwdt=damp*omega*dt;
        etau=exp(-hwdt);
        sinwdt=sin(omegaD*dt);
        coswdt=cos(omegaD*dt);
        //积分公式中8个常数
        double a11, a12, a21, a22;
        double b11, b12, b21, b22;
        a11=etau*(coswdt+damp*wratio*sinwdt);
        a12=etau*sinwdt/omegaD;
        a21=-1.0*etau*omega*wratio*sinwdt;
        a22=etau*(coswdt-damp*wratio*sinwdt);
        //
        double hw3dt, iw2, dampwDt;
        hw3dt=damp/pow(omega,3)/dt;
        iw2=1.0/pow(omega,2);
        dampwDt=(1-2*pow(damp,2))/pow(omega,2)/omegaD/dt;
        b11=etau*((iw2+2*hw3dt)*coswdt+(damp/omega/omegaD-dampwDt)*sinwdt)-2*hw3dt;
        b12=etau*(-2.0*hw3dt*coswdt+dampwDt*sinwdt)-iw2+2.0*hw3dt;
        b21=etau*(-1.0*iw2/dt*coswdt-(damp/omega/omegaD/dt+1.0/omegaD)*sinwdt)+iw2/dt;
        b22=etau*(iw2/dt*coswdt+damp/omega/omegaD/dt*sinwdt)-iw2/dt;
        //循环时程曲线求最大值
        int nSize=acc.size();
        double ui,vi;
        vi=-acc.front()*dt;
        ui=0;
        double maxAcc;
        maxAcc=fabs(2*damp*omega*acc.front()*dt);
        for(int j=1;j<nSize;j++)
        {
            double uNew, vNew, accPrev, accNext;
            accPrev=acc.at(j-1);
            accNext=acc.at(j);
            uNew=a11*ui+a12*vi+b11*accPrev+b12*accNext;
            vNew=a21*ui+a22*vi+b21*accPrev+b22*accNext;
            double accNew;
            accNew=-1.0*(2*damp*omega*vNew+pow(omega,2)*uNew);
            maxAcc=fmax(maxAcc,fabs(accNew));
            //
            ui=uNew;
            vi=vNew;
        }
        calSpec.addData(DataPoint(freq,maxAcc));
    }
}