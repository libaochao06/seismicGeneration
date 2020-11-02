#include "seisGenUtils.h"

void narrowBandAdjust(std::vector<double> &acc, const Spectrum &targetRsp, SeisGenPara params, std::ofstream &logFile)
{
    logFile<<">>>>采用窄带时程法对人工时程进行调整"<<std::endl;
    //
    typedef std::complex<double> cp;
    double dt, damp;//时间增量，阻尼比
    dt=params.dt;
    damp=targetRsp.getDamp();
    int nSize=acc.size();//人工时程长度
    std::vector<double> freqCtrl=targetRsp.getXSeries();//频率控制点数组
    double deltaS=0;//反应谱差值
    std::vector<double> deltaAcc;//增量时程
    //for(int i=0;i<freqCtrl.size();i++)
    for(int i=0;i<1;i++)
    {
        //
        deltaAcc.clear();
        std::vector<double> temp;
        double freq, maxAcc, maxTime, omega, omegac;//频率，最大加速度， 最大加速度出现时刻，角频率，角频率带宽
        freq=freqCtrl.at(i);
        if(freq<=3)
            continue;
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
        maxAcc=temp[1];
        maxTime=temp[0];
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
            if(fabs(t-maxTime)<1e-5)
            {
                value=deltaS*cos(omega*(t-maxTime));
            }
            else
            {
                double temp;
                temp=sin(omegac*(t-maxTime))/omegac/(t-maxTime);
                value=deltaS*pow(temp,4)*cos(omega*(t-maxTime));
            }
            deltaAcc.push_back(value);
        }
        // if(i==10)
        // {
        //     std::cout<<deltaS<<' '<<omegac<<' '<<omega<<' '<<maxTime<<std::endl;
        // }
        //反演计算地震窄带时程
        double deltaFreq;
        deltaFreq=1.0/(nSize-1)/dt;
        vector<cp> Haw;//时程传递函数
        Haw.clear();
        for(int j=0;j<nSize;j++)
        {
            double fi=j*deltaFreq;
            double ome,ome0;
            ome=PI2*fi;
            ome0=freq*PI2/sqrt(1-damp*damp);
            cp up(0,0);
            cp down(0,0);
            up.real(pow(ome0,2));
            up.imag(2*damp*ome0*ome);
            down.real(pow(ome0,2)-pow(ome,2));
            down.imag(2*damp*ome0*ome);
            Haw.push_back(up/down);
        }
        //求增量时程的傅里叶变换
        std::vector<cp> fourSeries, accSeries;
        fourSeries.clear();
        accSeries.clear();
        for(auto it=deltaAcc.begin();it!=deltaAcc.end();it++)
        {
            accSeries.push_back(cp(*it,0));
        }
        //由点值求系数
        fastFourierTrans(accSeries,fourSeries,-1);
        //计算系数与传递函数的比值
        for(int j=0;j<fourSeries.size();j++)
        {
            fourSeries.at(j)=fourSeries.at(j)/Haw.at(j);
        }
        //由系数求点值，得到窄带时程
        fastFourierTrans(fourSeries,accSeries,1);
        //在原始时程上叠加窄带时程
        for(int j=0;j<acc.size();j++)
        {
            //acc.at(j)=acc.at(j)+accSeries.at(j).real();
        }

        std::cout<<freqCtrl[1]-freqCtrl[0]<<' '<<freq<<' '<<maxTime<<std::endl;
        if(i==0)
        {
            std::ofstream ofileT("TimeHistory.txt", std::ios_base::out);
            // for(auto it=accSeries.begin();it!=accSeries.end();it++)
            // {
            //     double t;
            //     int loc=it-accSeries.begin();
            //     t=loc*params.dt;
            //     ofileT<<t<<"\t"<<it->real()<<'\t'<<acc[loc]<<'\t'<<deltaAcc[loc]<<std::endl;
            // }
            std::vector<cp> out=fourSeries;
            for(auto it=out.begin();it!=out.end();it++)
            {
                int loc=it-out.begin();
                ofileT<<deltaFreq*loc<<'\t'<<std::abs(*it)<<std::endl;
            }
            ofileT.close();
        }
    }
}