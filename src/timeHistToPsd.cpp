#include "seisGenUtils.h"

void timeHistToPsd(const std::vector<double> &acc, powerSpectrumDensity &psd, const SeisGenPara &params)
{
    //根据时程曲线计算平均功率密度谱
    //功率密度谱计算方法为 NUREG-0800 SRP Section 3.7.1 附录B中所规定的方法
    //本程序采用15%-85%的能量相对持时作为强震持时
    typedef std::complex<double> cp;
    double dt;//时间间隔
    int nSize;//时程数组长度
    nSize=acc.size();
    dt=params.dt;
    //计算时程总能量，15%能量，85%能量
    double energyTotal, energy15P, energy85P;//
    int loc15P,loc85P;//能量达到15%及85%时时程数组位置
    energyTotal=0;
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        energyTotal+=pow(*it,2);
    }
    energy15P=0.15*energyTotal;
    energy85P=0.85*energyTotal;
    double energyValue=0;
    loc15P=0;
    loc85P=0;
    for(int i=0;i<acc.size();i++)
    {
        energyValue+=pow(acc.at(i),2);
        if(energyValue>=energy15P && loc15P==0)
            loc15P=i;
        if(energyValue>=energy85P && loc85P==0)
            loc85P=i;
    }
    //计算强震持时
    double durationStrong;
    durationStrong=(loc85P-loc15P)*dt;
    //根据SRP 3.7.1 附录B中的要求，在计算时程的傅里叶幅值时，应截取强震段时程
    std::vector<double> accPsd=acc;
    for(int i=0;i<loc15P;i++)
        accPsd.at(i)=0;
    for(int i=loc85P+1;i<accPsd.size();i++)
        accPsd.at(i)=0;
    //
    double deltaFreq;
    deltaFreq=1.0/(nSize-1)/dt;

    std::vector<cp> fourSeries, accSeries;
    fourSeries.clear();
    accSeries.clear();

    for(auto it=accPsd.begin();it!=accPsd.end();it++)
    {
        accSeries.push_back(cp(*it, 0));
    }

    fastFourierTrans(accSeries, fourSeries, -1);
    //计算功率密度谱数组
    std::vector<double> psdVector;
    psdVector.clear();
    for(int i=0;i<nSize/2+1;i++)
    {
        double value;
        value=nSize*dt*abs(fourSeries.at(i));
        psdVector.push_back(pow(value,2)/PI/durationStrong);
    }
    //在[f-0.2f,f+0.2f]上对功率密度谱进行平均
    for(int i=0;i<nSize/2+1;i++)
    {
        double freq, valueRange;
        valueRange=0;
        freq=deltaFreq*i;
        int count=0;
        for(int j=0;j<nSize/2+1;j++)
        {
            double freqRange;
            freqRange=deltaFreq*j;
            if(freqRange>=0.8*freq &&freqRange<=1.2*freq)
            {
                valueRange+=psdVector.at(j);
                count++;
            }
            if(freqRange>1.5*freq)
            {
                break;
            }
        }
        psd.data.push_back(DataPoint(freq, valueRange/count));
    }
}