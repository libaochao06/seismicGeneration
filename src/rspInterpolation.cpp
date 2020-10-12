#include "seisGenUtils.h"

int rspInterpolation(const Spectrum &oRsp, Spectrum &tRsp,std::ofstream &logFile)
{
    //将oRsp赋值给tRsp
    tRsp=oRsp;
    if(tRsp.getDataSize()==0)
    {
        logFile<<">>反应谱"<<oRsp.name<<"中无数据，插值中止"<<std::endl;
        return 1;
    }
    std::string str="Target_";
    tRsp.name=str+oRsp.name;
    //将反应谱横坐标统一转化为频率
    if(tRsp.getXType()!=spectrumXType::Freq)
    {
        for(auto &dp:tRsp.getData())
        {
            dp.setX(1.0/dp.getX());
        }
    }
    //将反应谱纵坐标统一转化为加速度
    if(tRsp.getYType()!=spectrumYType::Accel)
    {
        for(auto &dp:tRsp.getData())
        {
            dp.setY(dp.getY()*pow(PI2*dp.getX(),(double)tRsp.getYType()));
        }
    }

    //将反应谱按频率由小到大排序
    tRsp.sortData();
    //根据GB50267 表3.4.1计算标准规定的频率控制点数组
    vector<double> freqCtrlPoints;
    //目标反应谱中频率的最小值与最大值
    double freqMin, freqMax;
    freqMin=tRsp.getData().front().getX();
    freqMax=tRsp.getData().back().getX();
    //控制点数组的频率最小值不应大于法规规定的频率最小值，即0.2Hz
	freqMin = std::min(freqMin, freqCtrlStandard::points.front());
	//控制点数组的频率最大值不应小于法规规定的频率最大值，即33Hz
	freqMax = std::max(freqMax, freqCtrlStandard::points.back());
    
    //freqCtrlPoints.push_back(freqMin);

    for(int i=1;i<freqCtrlStandard::points.size();i++)
    {
        //频率增量位置
        int locInc=i-1;
        //当前频率控制点和上一个频率控制点
        double freq, freqPrev;
        freq=freqCtrlStandard::points[i];
        freqPrev=freqCtrlStandard::points[i-1];
        if(i==1)
            freqPrev=freqMin;
        if(i==freqCtrlStandard::points.size()-1)
            freq=freqMax;
        //频率增量个数，向上取整
        int numInc;
        numInc=ceil((freq-freqPrev)/freqCtrlStandard::increments[locInc]);
        //实际频率增量
        double deltaFreq;
        deltaFreq=(freq-freqPrev)/numInc;
        for(int j=0;j<numInc;j++)
        {
            freqCtrlPoints.push_back(freqPrev+j*deltaFreq);
        }
    }
    //标准要求频率控制点数组与目标反应谱频率合并并插值
    for(auto it=freqCtrlPoints.begin()+1;it!=freqCtrlPoints.end()-1;it++)
    //频率控制点数组第一个值与最后一个值与目标谱中第一个频率值和最后一个频率值相同
    {
        int dataSize=tRsp.getDataSize();
        for(int i=0;i<dataSize-1;i++)
        {
            double freqPrev, freqNext, accelPrev, accelNext;
            freqPrev=tRsp[i].getX();
            freqNext=tRsp[i+1].getX();
            if(*it>1.01*freqPrev && *it<0.99*freqNext)
            {
                accelPrev=tRsp[i].getY();
                accelNext=tRsp[i+1].getY();
                double slop, value;
                slop=(log10(accelNext)-log10(accelPrev))/(log10(freqNext)-log10(freqPrev));
                value=slop*(log10(*it)-log10(freqPrev))+log10(accelPrev);
                value=pow(10,value);
                tRsp.getData().push_back(DataPoint(*it,value));
                break;
            }
        }
    }
    tRsp.sortData();
    return 0;
}