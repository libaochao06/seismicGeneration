#include "seisGenUtils.h"

void rspMatching(std::vector<double> &acc, const Spectrum &targetRsp, SeisGenPara params, std::ofstream &logFile)
{
    /**
     * @brief 人工时程曲线目标谱拟合函数
     */
    typedef std::complex<double> cp;
    //峰值加速度
    double maxAcc=0;
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        maxAcc=fmax(maxAcc,fabs(*it));
    }
    //频率增量
    double deltaFreq=1.0/(acc.size()-1)/params.dt;
    //计算反应谱
    Spectrum calSpec(spectrumXType::Freq, spectrumYType::Accel, targetRsp.getDamp());
    timeHistToSpectrum(acc, targetRsp.getXSeries(), params.dt, calSpec);
    int nCycle=0;
    const int MaxNumCycle=40;
    bool isChecked;
    isChecked=targetRspEnvCheck(targetRsp, calSpec,logFile);
    while(!isChecked)
    {
        logFile<<"初始时程不满足目标反应谱包络要求，开始进行迭代修正！"<<std::endl;
        nCycle++;
        logFile<<"第"<<nCycle<<"轮迭代修正开始！"<<std::endl;
        //傅里叶振幅谱
        std::vector<double> ampFourier, phiFourier;
        ampFourier.clear();
        phiFourier.clear();
        std::vector<cp> fourSeries, accSeries;
        fourSeries.clear();
        accSeries.clear();
        accSeries.push_back(cp(0,0));
        for(int i=1;i<acc.size();i++)
        {
            accSeries.push_back(cp(acc.at(i),0));
        }
        //由点值求系数
        fastFourierTrans(accSeries, fourSeries, -1);
        for(auto it=fourSeries.begin();it!=fourSeries.end();it++)
        {
            ampFourier.push_back(abs(*it));
            phiFourier.push_back(std::arg(*it));
        }
        //频率区间
        std::vector<double> freqCtrl;
        freqCtrl=targetRsp.getXSeries();
        for(auto it=freqCtrl.cbegin();it!=freqCtrl.cend();it++)
        {
            double freq1, freq2, ratio;
            int loc;
            loc=it-freqCtrl.cbegin();
            ratio=targetRsp[loc].getY()/calSpec[loc].getY();
            ratio=fabs(ratio)*(1+0.9*targetRsp.getDamp());
            //确定频率修正区间
            if(it==freqCtrl.cbegin())
            {
                freq1=*it;
                freq2=0.5*(*it+(*(it+1)));
            }
            else if(it==freqCtrl.cend()-1)
            {
                freq1=0.5*(*it+(*(it-1)));
                freq2=*it*1.01;
            }
            else
            {
                freq1=0.5*(*it+(*(it-1)));
                freq2=0.5*(*it+(*(it+1)));
            }
            // double ratio1, ratio2;
            // ratio1=targetRsp.getValueByX(freq1)/calSpec.getValueByX(freq1);
            // ratio2=targetRsp.getValueByX(freq2)/calSpec.getValueByX(freq2);
            //在频率区间内进行傅里叶幅值调整
            for(int j=1;j<fourSeries.size()/2;j++)
            {
                double freq;
                freq=deltaFreq*j;
                if(freq>=freq1 &&freq<freq2)
                {
                    //ratio=(ratio2-ratio1)/(freq2-freq1)*(freq-freq1)+ratio1;
                    //ratio=fabs(ratio)*(1.0+0.3*targetRsp.getDamp());
                    //fourSeries.at(j)=fourSeries.at(j)*ratio;
                    ampFourier.at(j)=ampFourier.at(j)*ratio;
                }
                if(freq>freq2)
                    break;
            }
        }
        
        fourSeries[0]=cp(0,0);
        //重新计算系数数组
        for(int i=1;i<fourSeries.size()/2;i++)
        {
            double phi=phiFourier.at(i);
            fourSeries.at(i)=ampFourier.at(i)*cp(cos(phi),sin(phi));
            fourSeries.at(fourSeries.size()-1)=std::conj(fourSeries.at(i));
        }
        fourSeries.at(fourSeries.size()/2)=ampFourier.at(ampFourier.size()/2);
        //由系数求点值
        fastFourierTrans(fourSeries, accSeries, 1);
        //计算调整后的时程曲线
        for(auto it=accSeries.begin();it!=accSeries.end();it++)
        {
            acc.at(it-accSeries.begin())=it->real();
        }
        //基线调整
        baselineAdjust(acc, params.dt);
        //峰值调整
        //peakAdjust(acc, maxAcc);
        //非平稳包络曲线包络
        // std::vector<double> env;
        // envelopeFuncCal(params.tDuration,params.tRise, params.tDrop, params.dt, acc.size(), env);
        // for(int i=0;i<acc.size();i++)
        // {
        //     acc.at(i)=acc.at(i)*env.at(i);
        // }

        timeHistToSpectrum(acc, targetRsp.getXSeries(), params.dt, calSpec);
        isChecked=targetRspEnvCheck(targetRsp,calSpec, logFile);
        if(isChecked)
        {
            logFile<<"经过"<<nCycle<<"轮迭代，计算反应谱满足目标反应谱包络要求"<<std::endl;
            break;
        }


        if(nCycle>=MaxNumCycle)
            break;
    }
    std::ofstream ofileT("Spectrum.txt", std::ios_base::out);
	for (int i=0;i<targetRsp.getDataSize();i++)
	{
	    double freq;
	    freq=targetRsp[i].getX();
	    ofileT << freq << '\t' << targetRsp[i].getY() << '\t' <<calSpec[i].getY()<<'\n';
	}
    ofileT.close();
}