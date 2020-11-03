#include "seisGenUtils.h"

bool rspToPsdVanmarcke(const Spectrum &tRsp, powerSpectrumDensity &psd, double deltaFreq, double Td, int N, std::ofstream &logFile)
/**
 * @brief Vanmarcke 方法反应谱转功率密度谱
 *      1. 使用Vanmarcke方法将反应谱转化为功率密度谱，功率密度谱为单侧功率密度谱，其横坐标为频率，单位Hz
 *      当deltaFreq不为0时，则使用该频率间隔，当deltaFreq=0时，则使用目标反应谱的频率点进行功率谱计算    
 *      为后面计算的方便，尽量使用等频率间隔计算功率谱，即提供deltaFreq。
 *      2. 最终输出的功率密度谱数据点个数至少应为nFour/2个，nFour为傅里叶变换数据点个数，若数据点不足nFour/2个，则采用后续补零的办法补齐
 */
{
    double omegaMax, omegaMin, freqMax, freqMin;//角频率及频率上、下限
    const double probability=0.85;//超越概率
    double omega=0, freq;//角频率，频率
    double gSum=0;//功率密度函数对角频率积分求和
    double damp=tRsp.getDamp();
    psd.name=tRsp.name+"_Psd";
    logFile<<">>使用Vanmarcke方法由反应谱"<<tRsp.name<<"计算生成功率密度谱"<<psd.name<<"开始"<<std::endl;
    //频率上、下限取目标谱频率上、下限
    freqMin=tRsp.getData().front().getX();
    freqMax=tRsp.getData().back().getX();
    freqMin=fmax(freqMin, 0.05);//设置频率下限不得小于0.05Hz
    omegaMax=PI2*freqMax;
    omegaMin=PI2*freqMin;

    //当omega=0时，功率密度G(w)=0
    psd.data.push_back(DataPoint(0,0));
    int ii=1;
    //循环计算各频率下的功率密度
    while(omega<omegaMax)
    {
    /**
    * 如果给定了频率增量步deltaFreq，则每次omega增加2*PI*deltaFreq
	* 如果没有给定频率增量步，则根据目标反应谱的频率值计算当前omega值
	* 在循环中计算当前角频率下的功率密度谱值
    */
        if(deltaFreq!=0)
        {
            omega+=PI2*deltaFreq;
        }
        else
        {
            if(ii<tRsp.getDataSize())
            {
                omega=PI2*tRsp[ii].getX();
                ii++;
            }
            else
            {
                omega=omegaMax;
            }
        }

        if(omega<omegaMin)
        {
            if(omega<0)
            {
               logFile<<"错误！当前角频率<0！"<<std::endl;
               return false;
            }
            //当角频率小于下限时，功率密度谱值为0
            psd.data.push_back(DataPoint(omega/PI2,0));
            continue;
        }
        else if(omega>omegaMax)
        {
            //当前角频率超过上限时，循环中止
            omega=omegaMax;
            break;
        }
        double rspVal, r, psdVal;//反应谱值，峰值系数，功率密度谱值
        freq=omega/PI2;
        rspVal=tRsp.getValueByX(freq);
        r=rPeak(probability, Td, freq, damp);
        double denominator, sqMeanSqDev;//功率密度谱计算公式中的分母和反应谱值的均方差的平方
        denominator=omega*(PI/4/damp-1);
        sqMeanSqDev=pow(rspVal/r, 2);
        psdVal=(sqMeanSqDev-gSum)/denominator;
        //对功率密度谱进行累积求和
        if(omega==omegaMin)
        {
            gSum=0.5*omegaMin*psdVal;//起始初值
        }
        else
        {
            if(deltaFreq!=0)
                gSum+=PI2*deltaFreq*psdVal;
            else
            {
                gSum+=PI2*fabs(tRsp[ii-1].getX()-tRsp[ii-2].getX())*psdVal;
            }
        }
        psd.data.push_back(DataPoint(freq, psdVal));
        
        if(ii>N)
            break;
    }

    //当功率密度谱数据不足N个时，后续用0补充至N个
    int i=psd.data.size();
    double df;
    if(deltaFreq!=0)
    {
        df=deltaFreq;
    }
    else
    {
        double fN;
        fN=(N-1)/2.0/Td;
        df=(fN-tRsp.getData().back().getX())/(N-i);
    }
    
    for(;i<N;i++)
    {
        psd.data.push_back(DataPoint(i*df,0));
    }
    //
    
    logFile<<">>使用Vanmarcke方法由反应谱"<<tRsp.name<<"计算生成功率密度谱"<<psd.name<<"完成"<<std::endl;
    return true;
}