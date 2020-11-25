#include "seisGenUtils.h"

void targetPsdAdjust(std::vector<double> &acc, const powerSpectrumDensity &psd, const SeisGenPara &params, const std::vector<double> &phaseAngle, std::ofstream &logFile)
{
    typedef std::complex<double> cp;
    //按照法规要求计算时程的功率密度谱
    powerSpectrumDensity calPsd(spectrumXType::Freq, spectrumYType::Accel, "calAccPsd");
    timeHistToPsd(acc, calPsd, params);
    // //*********************************************
    // std::ofstream ofilePsd("powerSpectrumDensity.txt",std::ios_base::out);
    // for(int i=0;i<calPsd.data.size();i++)
    // {
    //     double freq, valueT, valueC;
    //     freq=calPsd.data.at(i).getX();
    //     valueT=psd.data[i].getY();
    //     valueC=calPsd.data[i].getY();
    //     ofilePsd<<freq<<'\t'<<valueT<<'\t'<<valueC<<std::endl;
    // }
    // ofilePsd.close();
    // //***********************************************************
    int nSize=acc.size();
    double dt=params.dt;
    double deltaFreq=1.0/(nSize-1)/dt;
    //
    std::vector<cp>  fourSeries, accSeries;

    for(auto it=acc.begin();it!=acc.end();it++)
    {
        accSeries.push_back(cp(*it,0));
    }

    //求时程的傅里叶系数
    fastFourierTrans(accSeries,fourSeries, -1);
    //对时程的傅里叶系数按照GB50267 4.3.4节中要求进行调整
    for(int i=0;i<nSize/2;i++)
    {
        double freq, calValue, targetValue;//当前频率，计算功率密度，目标功率密度
        freq=deltaFreq*i;
        if(freq>=0.3 && freq<=24)//对0.3到24Hz区间内的功率密度谱进行调整
        {
            calValue=calPsd.data.at(i).getY();
            targetValue=psd.data.at(i).getY();
            if(calValue<0.8*targetValue)
            {
                fourSeries.at(i)=fourSeries.at(i)*sqrt(targetValue*0.8/calValue);
                fourSeries.at(i+1)=fourSeries.at(i+1)*sqrt(targetValue*0.8/calValue);
                fourSeries.at(i-1)=fourSeries.at(i-1)*sqrt(targetValue*0.8/calValue);
            }
        }
        if(freq>25)
            break;
    }
    // std::vector<double> ampFourier;
    // for(auto it=fourSeries.begin();it!=fourSeries.end();it++)
    // {
    //     ampFourier.push_back(abs(*it));
    // }
    // fourSeries.at(0)=cp(0,0);
    // for(int i=1;i<nSize/2;i++)
    // {
    //     double phi=phaseAngle.at(i-1);
    //     fourSeries.at(i)=ampFourier.at(i)*cp(cos(phi), sin(phi));
    //     fourSeries.at(nSize-i)=std::conj(fourSeries.at(i));
    // }
    // fourSeries.at(nSize/2)=cp(ampFourier.at(nSize/2),0);
    //根据调整后的傅里叶系数计算新的时程曲线
    fastFourierTrans(fourSeries, accSeries, 1);
    for(int i=0;i<nSize;i++)
    {
        acc.at(i)=accSeries.at(i).real();
    }
    //基线调整
    // baselineAdjust(acc, params.dt);
}