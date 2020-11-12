#include "seisGenUtils.h"

void fourierAmplitudeAdjust(std::vector<double> &acc, const Spectrum &targetRsp, const SeisGenPara &params, std::ofstream &logFile)
{
    /**
     * @brief 人工时程曲线目标谱拟合函数
     */
    logFile<<">>>>采用傅里叶幅值谱调整法对人工时程进行调整"<<std::endl;
    //
    double maxAcc=0;
    maxAcc=maxAbsOfTimeHist(acc);
    typedef std::complex<double> cp;
    //频率增量
    double deltaFreq=1.0/(acc.size()-1)/params.dt;
    //计算反应谱
    Spectrum calSpec(spectrumXType::Freq, spectrumYType::Accel, targetRsp.getDamp());
    timeHistToSpectrum(acc, targetRsp.getXSeries(), params.dt, calSpec);
    //傅里叶振幅谱、相位谱
    std::vector<double> ampFourier, phiFourier;
    ampFourier.clear();
    phiFourier.clear();
    std::vector<cp> fourSeries, accSeries;
    fourSeries.clear();
    accSeries.clear();
    accSeries.push_back(cp(0, 0));
    for (int i = 1; i < acc.size(); i++)
    {
        accSeries.push_back(cp(acc.at(i), 0));
    }
    //由点值求系数
    fastFourierTrans(accSeries, fourSeries, -1);
    //求傅里叶幅值和相位角
    // phaseAngleFuncCal(params, acc.size(), phiFourier, phaseAngleCalMethod::EnvFunc);
    // phiFourier.insert(phiFourier.begin(), 0.0);
    //
    for (auto it = fourSeries.begin(); it != fourSeries.end(); it++)
    {
        ampFourier.push_back(abs(*it));
        phiFourier.push_back(std::arg(*it));
    }

    //频率区间
    std::vector<double> freqCtrl;
    freqCtrl = targetRsp.getXSeries();
    for (auto it = freqCtrl.cbegin(); it != freqCtrl.cend(); it++)
    {
        double freq1, freq2, ratio;
        int loc;
        loc = it - freqCtrl.cbegin();
        ratio = targetRsp[loc].getY() / calSpec[loc].getY();
        ratio = fabs(ratio) * (1 + 0.3 * targetRsp.getDamp());

        // ratio=fabs(ratio)*1.05;
        // if(ratio<0.7)
        // {
        //     ratio=fabs(ratio)*(1-0.3*targetRsp.getDamp());
        // }
        // else if(ratio>1.1)
        // {
        //     ratio=fabs(ratio)*(1+targetRsp.getDamp());
        // }
        // else
        // {
        //      ratio=fabs(ratio)*(1+0.3*targetRsp.getDamp());
        // }

        //确定频率修正区间
        if (it == freqCtrl.cbegin())
        {
            freq1 = *it;
            freq2 = 0.5 * (*it + (*(it + 1)));
        }
        else if (it == freqCtrl.cend() - 1)
        {
            freq1 = 0.5 * (*it + (*(it - 1)));
            freq2 = *it * 1.05;
        }
        else
        {
            freq1 = 0.5 * (*it + (*(it - 1)));
            freq2 = 0.5 * (*it + (*(it + 1)));
        }
        // double ratio1, ratio2;
        // ratio1=targetRsp.getValueByX(freq1)/calSpec.getValueByX(freq1);
        // ratio2=targetRsp.getValueByX(freq2)/calSpec.getValueByX(freq2);
        //std::cout<<ratio1<<' '<<ratio<<' '<<ratio2<<std::endl;
        //在频率区间内进行傅里叶幅值调整
        for (int j = 0; j < fourSeries.size() / 2; j++)
        {
            double freq;
            freq = deltaFreq * j;
            // if (freq < freqCtrl.front())
            //     ampFourier.at(j) = 0;
            //std::cout<<deltaFreq<<std::endl;
            if (freq >= freq1 && freq < freq2)
            {
                // double ratioF;
                // if(freq<=*it)
                // {
                //     ratioF=(ratio-ratio1)/(*it-freq1)*(freq-freq1)+ratio1;
                //     ratioF=fabs(ratioF)*(1+0.1*targetRsp.getDamp());
                // }
                // else
                // {
                //     ratioF=(ratio2-ratio)/(freq2-*it)*(freq-*it)+ratio;
                //     ratioF=fabs(ratioF)*(1+0.01*targetRsp.getDamp());
                // }
                //ratio=fabs(ratio)*(1.0+0.3*targetRsp.getDamp());
                // fourSeries.at(j)=fourSeries.at(j)*ratio;
                ampFourier.at(j) = ampFourier.at(j) * ratio;
            }
            if (freq > freq2)
                break;
        }
    }

        //重新计算系数数组
        //傅里叶幅值调整完成后，由新的幅值计算傅里叶系数数组，相位角保持不变
        fourSeries[0]=ampFourier.front()*cp(cos(phiFourier.front()), sin(phiFourier.front()));
        for(int i=1;i<fourSeries.size()/2;i++)
        {
            double phi=phiFourier.at(i);
            fourSeries.at(i)=ampFourier.at(i)*cp(cos(phi),sin(phi));
            fourSeries.at(fourSeries.size()-i)=std::conj(fourSeries.at(i));
        }
        fourSeries.at(fourSeries.size()/2)=ampFourier.at(ampFourier.size()/2);
        //由系数求点值
        fastFourierTrans(fourSeries, accSeries, 1);
        //计算调整后的时程曲线
        for(auto it=accSeries.begin();it!=accSeries.end();it++)
        {
            acc.at(it-accSeries.begin())=it->real();
        }
}