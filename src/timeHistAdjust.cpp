#include "seisGenUtils.h"

void timeHistAdjust(std::vector<double> &acc, const Spectrum &targetRsp, SeisGenPara params, std::ofstream &logFile)
{
    //峰值加速度
    double maxAcc=0;
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        maxAcc=fmax(maxAcc,fabs(*it));
    }
    //定义当前循环次数以及最大允许循环次数
    int nCycle=0;
    const int MaxNumCycle=10;
    //计算反应谱
    Spectrum calSpec(spectrumXType::Freq, spectrumYType::Accel, targetRsp.getDamp());
    timeHistToSpectrum(acc, targetRsp.getXSeries(), params.dt, calSpec);
    //判断计算反应谱与目标反应之间是否满足法规要求
    bool isChecked;
    isChecked=targetRspEnvCheck(targetRsp, calSpec,logFile);
    while(!isChecked)
    {
        logFile<<">>初始时程不满足目标反应谱包络要求，开始进行迭代修正！"<<std::endl;
        nCycle++;
        logFile<<">>第"<<nCycle<<"轮迭代修正开始！"<<std::endl;
        //傅里叶幅值调整
        fourierAmplitudeAdjust(acc, targetRsp, params, logFile);
        //窄带时程调整
        //narrowBandAdjust(acc, targetRsp, params, logFile);
        //重新计算反应谱并对其按照法规要求进行检查
        timeHistToSpectrum(acc, targetRsp.getXSeries(), params.dt, calSpec);
        isChecked=targetRspEnvCheck(targetRsp, calSpec, logFile);
        if(isChecked)
        {
            logFile<<">>经过"<<nCycle<<"轮迭代，计算反应谱满足目标反应谱包络性要求"<<std::endl;
            break;
        }
        if(nCycle>=MaxNumCycle)
        {
            logFile<<"迭代次数达到最大允许迭代次数，迭代终止！"<<std::endl;
            break;
        }
    }
    std::ofstream ofileT("Spectrum.txt", std::ios_base::out);
    for(int i=0;i<targetRsp.getDataSize();i++)
    {
        double freq, valueT, valueC;
        freq=targetRsp[i].getX();
        valueT=targetRsp[i].getY();
        valueC=calSpec[i].getY();
        ofileT<<freq<<'\t'<<valueT<<'\t'<<valueC<<std::endl;
    }
    ofileT.close();
}