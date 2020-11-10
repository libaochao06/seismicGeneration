#include "seisGenUtils.h"

void narrowBandAdjust(std::vector<double> &acc, const Spectrum &targetRsp, const SeisGenPara &params, std::ofstream &logFile)
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
    for(int i=0;i<freqCtrl.size();i++)
    // for(int i=0;i<1;i++)
    {
        //
        deltaAcc.clear();
        std::vector<double> temp;
        double freq, maxAcc, maxTime, omega, omegac;//频率，最大加速度， 最大加速度出现时刻，角频率，角频率带宽
        freq=freqCtrl.at(i);
        double targetValue=targetRsp[i].getY();
        targetValue*=(1+0.005);
        //***************
        //求最大响应及其出现时刻
        maxResp(acc, freq, damp, dt, temp);
        maxAcc=temp[1];
        maxTime=temp[0];
        if((fabs(fabs(maxAcc)-targetValue)/targetValue)<0.3)
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
        omegac=fmax(omegac,0.05*PI2);
        omegac=fmin(omegac, 0.5*PI2);
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
                value=deltaS;
            }
            else
            {
                double temp;
                int power;
                if(freq<2.0)
                    power=4;
                else if(freq>=2.0 &&freq<15)
                    power=4;
                else
                {
                    power=4;
                }
                
                temp=sin(omegac*(t-maxTime))/omegac/(t-maxTime);
                if(omegac<0.1)
                {
                    value=deltaS*pow(temp,power)*cos(omega*(t-maxTime));
                }
                else
                {
                    value=deltaS*pow(temp,power)*cos(omega*(t-maxTime));
                }
            }
            deltaAcc.push_back(value);
        }
        // if(i==10)
        // {
        //    std::cout<<deltaS<<' '<<omegac<<' '<<omega<<' '<<i<<std::endl;
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
        fourSeries.front()=fourSeries.front()/Haw.front();
        for(int j=1;j<nSize/2;j++)
        {
            fourSeries.at(j)=fourSeries.at(j)/Haw.at(j);
            fourSeries.at(nSize-j)=std::conj(fourSeries.at(j));
        }
        fourSeries.at(nSize/2)=fourSeries.at(nSize/2)/abs(Haw.at(nSize/2));
        //由系数求点值，得到窄带时程
        fastFourierTrans(fourSeries,accSeries,1);
        for(int j=0;j<accSeries.size();j++)
        {
            deltaAcc.at(j)=accSeries.at(j).real();
        }
        //在原始时程上叠加窄带时程
        for(int j=0;j<acc.size();j++)
        {
            acc.at(j)=acc.at(j)+deltaAcc.at(j);
        }
        //
        // std::ofstream ofileT("TimeHistory.txt", std::ios_base::out);
        // for(auto it=Haw.begin();it!=Haw.end();it++)
        // {
        //     double t;
        //     //t=(it-deltaAcc.begin())*params.dt;
        //     ofileT<<it->real()<<"\t"<<it->imag()<<std::endl;
        // }
        // ofileT.close();
    }
}