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
    double Td=(nSize-1)*dt;
    std::vector<double> freqCtrl=targetRsp.getXSeries();//频率控制点数组
    double deltaS=0;//反应谱差值
    std::vector<double> deltaAcc;//增量时程
    std::vector<double> addAcc;//所有增量时程的和
    for(int i=0;i<nSize;i++)
    {
        addAcc.push_back(0);
    }
    std::cout<<freqCtrl.size()<<std::endl;
    logFile<<"按照频率控制点计算窄带增量时程开始"<<std::endl;
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
        //计算窄带时程衰减因子
        double attenufactor=0;
        double t1=0, t2=0;
        //增大omegac直至衰减因子满足要求
        while(1)
        {
            if(maxTime<(Td-maxTime))
            {
                double value;
                value=sin(omegac*(-maxTime))/omegac/(-maxTime);
                attenufactor=pow(value,4);
                t1=-PI/omegac+maxTime;
            }
            else
            {
                double value;
                value=sin(omegac*(Td-maxTime))/omegac/(Td-maxTime);
                attenufactor=pow(value,4);
                t2=PI/omegac+maxTime;
            }
            //如果omegac不合适，则适当增加omegac
            if(attenufactor>0.03 || t1<0 || t2>Td)
            {
                omegac=omegac*1.1;
            }
            else
            {
                break;
            }
        }
        
        // omegac=fmax(omegac,0.05*PI2);
        // omegac=fmin(omegac, 0.5*PI2);
        //求反应谱差值 deltaS=sgn[amax]*[St-Sa], St为目标反应谱，Sa为计算反应谱
        // x>0 => sgn=1
        // x=0 => sgn=0
        // x<0 => sgn=-1
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
        narrowBandAcc(deltaS,omegac,omega,maxTime, nSize, dt, deltaAcc);
        
        //反演计算地震窄带时程
        double deltaFreq;
        deltaFreq=1.0/Td;
        vector<cp> Haw;//时程传递函数
        Haw.clear();
        //无阻尼角频率
        double ome0;
        ome0=freq*PI2/sqrt(1-damp*damp);
        transFunction(ome0, PI2*deltaFreq, damp, nSize, Haw);
        //由所构造的窄带时程计算反演地震动时程
        nBandAccRevByTransFunc(deltaAcc, Haw);
        
        /**
         * 对窄带时程进行校核，如果存在端头振荡问题，则对其进行修正
         */
        nBandAccEndAdjust(deltaAcc, damp, dt, deltaS, maxTime, omegac, omega);
       
        //在原始时程上叠加窄带时程
        for(int j=0;j<addAcc.size();j++)
        {
            addAcc.at(j)=addAcc.at(j)+deltaAcc.at(j);
        }
        std::cout<<i<<std::endl;
    }
    std::ofstream ofileT("TimeHistory.txt", std::ios_base::out);
    for(auto it=addAcc.begin();it!=addAcc.end();it++)
    {
        double t;
        t=(it-addAcc.begin())*params.dt;
        ofileT<<t<<"\t"<<*it<<std::endl;
    }
    ofileT.close();
}