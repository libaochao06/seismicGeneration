#include "seismicGeneration.h"
#include "seisGenUtils.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    //人工时程曲线
    vector<double> accTimeHist;
    //判断输入参数个数是否正确
    if(argc<2)
    {
        cout<<"缺少输入参数，请检查输入参数是否正确！"<<endl;
        abort;
    }
    //读取输入文件
    string infileName;//输入文件名
    infileName=argv[1];
    
    SeisGenPara params;//全局计算参数
    ofstream logFile("output.log", std::ios_base::out);//log文件
    vector<Spectrum> specs;//反应谱数据数组

    readInput(infileName, params, specs, logFile);//读取输入文件函数

    //****************************************************************
    // 人工时程计算 Step:1 原始谱数据处理及全局计算参数计算
    // for(auto it=specs.begin();it!=specs.end();it++)
    // {
    //     changeSpectrumUnit(*it, 1);
    // }
    // 计算总持时，频率间隔
    double Td, deltaFreq;
    //傅里叶变换总点数
    int nFour;
    nFour=FFTParams(params.tDuration, params.dt, Td, deltaFreq);
    //****************************************************************
    // 人工时程计算 Step:2 按照规范要求对原始反应谱进行插值，并将插值结果存
    // 放至targetRsp数组中
    logFile<<"*********************************"<<std::endl;
    logFile<<">>原始反应谱插值开始"<<std::endl;
    vector<Spectrum> targetRsp;
    int count=0;
    for(auto &sp:specs)
    {
        Spectrum temp=sp;
        logFile<<">>反应谱"<<sp.name<<"插值开始"<<std::endl;
        int rspFlag;
        rspFlag=rspInterpolation(sp, temp,logFile);
        if(rspFlag!=0)
        {
            logFile<<">>反应谱"<<sp.name<<"插值失败"<<std::endl;
        }
        else
        {
            logFile<<">>反应谱"<<sp.name<<"插值完成，目标反应谱："<<temp.name<<std::endl;
            targetRsp.push_back(temp);
            count++;
        }
    }
    logFile<<">>原始反应谱插值结束，共成功插值"<<count<<"组反应谱数据"<<endl;
    logFile<<"*********************************"<<std::endl;

    // 人工时程计算 Step:3 人工时程计算开始，从此步开始为每条反应谱单独生成人工时程
    logFile<<"*********************************"<<std::endl;
    logFile<<">>人工时程计算开始，从此步开始为每条反应谱单独生成人工时程"<<std::endl;
    vector<powerSpectrumDensity> targetPsd;
    for(auto &tRsp : targetRsp)
    {
        logFile<<">>反应谱"<<tRsp.name<<"人工时时程拟合开始"<<endl;
        // 人工时程计算 Step:3.1 由目标反应谱计算近似功率密度谱
        logFile<<">>反应谱"<<tRsp.name<<"功率密度谱计算开始"<<endl;
        powerSpectrumDensity psd(spectrumXType::Freq, spectrumYType::Accel);
        switch (params.flag)
        {
        case PSDFlag::RG160:
            /* To Do */
            break;
        case PSDFlag::Vanmarcke:
            rspToPsdVanmarcke(tRsp, psd, deltaFreq, Td, nFour/2, logFile);
            break;
        case PSDFlag::Kaul:
            /* To Do */
            break;
        default:
            break;
        }
        targetPsd.push_back(psd);
        // ofstream ofilePsd("powerSpectrumDensity.txt",ios_base::out);
        // for(auto it=psd.data.begin();it!=psd.data.end();it++)
        // {
        //     int loc=it-psd.data.begin();
        //     ofilePsd<<it->getX()<<'\t'<<it->getY()<<'\t'<<psd2.data.at(loc).getY()<<endl;
        // }
        // ofilePsd.close();
        // 人工时程包络曲线计算
        logFile<<">>人工时程包络曲线计算开始"<<endl;
        vector<double> envFunc;
        envelopeFuncCal(params.tDuration, params.tRise, params.tDrop, params.dt, nFour, envFunc);
        logFile<<">>人工时程包络曲线计算完成"<<endl;
        //人工时程相位角计算
        logFile<<">>人工时程相位角计算开始"<<endl;
        vector<double> phaseAngle;
        phaseAngleFuncCal(params, nFour, phaseAngle, phaseAngleCalMethod::EnvFunc);

        logFile<<">>人工时程相位角计算完成"<<endl;
        //人工时程计算 Step:3.2 初始人工时程计算
        logFile<<">>初始人工时程计算开始"<<endl;
        initAcc(psd, phaseAngle, nFour, accTimeHist);
        //采用包络曲线对人工时程进行包络
        logFile<<">>>>初始人工时程包络曲线包络调整"<<endl;
        accEnvelop(accTimeHist, envFunc);
        //峰值加速度调整
        logFile<<">>>>初始人工时程峰值加速度调整"<<endl;
        double desiredAmp;
        desiredAmp=params.maxAccels[&tRsp-&targetRsp[0]]*G;
        // timeHistScale(accTimeHist, desiredAmp/maxAbsOfTimeHist(accTimeHist));
        peakAdjust(accTimeHist, desiredAmp);
        // peakReduction(accTimeHist, desiredAmp, envFunc);
        //基线调整
        logFile<<">>>>初始人工时程基线调整"<<endl;
        // baselineAdjust(accTimeHist, params.dt);
        logFile<<">>初始人工时程计算完成"<<endl;
        //目标反应谱包络性调整
        logFile<<">>人工时程反应谱包络性调整开始"<<endl;
        Spectrum calSpec(spectrumXType::Freq, spectrumYType::Accel, tRsp.getDamp());
        timeHistToSpectrum(accTimeHist, tRsp.getXSeries(), params.dt, calSpec);
        //判断计算反应谱与目标反应之间是否满足法规要求
        bool isChecked;
        //根据反应谱和功率密度谱对人工时程进行傅里叶幅值调整
        for(int ii=0;ii<200;ii++)
        {
            
            fourierAmplitudeAdjust(accTimeHist, tRsp, params, phaseAngle, logFile);
            targetPsdAdjust(accTimeHist, psd, params, phaseAngle, logFile);
            timeHistToSpectrum(accTimeHist, tRsp.getXSeries(), params.dt, calSpec);
            rspError error=errorCalRspToTargetRsp(tRsp, calSpec);
            if(fabs(error.maxNegErr)<0.05)
            {
                for(auto it=accTimeHist.begin();it!=accTimeHist.end();it++)
                {
                    *it=(*it)*(1+fabs(error.maxNegErr));
                }
            }
            // targetRspEnvCheck(error, logFile);
            //当计算反应谱与目标反应谱间相对误差的均方差小于0.03时，则退出调整
            if(error.errSquareRoot<0.03)
            {
                std::cout<<ii<<std::endl;
                break;
            }
        }
        std::cout<<"Narrow Band Adjust!"<<std::endl;
        narrowBandAdjust(accTimeHist, tRsp, params, logFile);
        peakAdjust(accTimeHist, desiredAmp);
        peakReduction(accTimeHist, desiredAmp, envFunc);
       
        // accEnvelop(accTimeHist, envFunc);
        baselineAdjust(accTimeHist, params.dt);
        // timeHistScale(accTimeHist, 1.02);
        //窄带时程调整
        // for(int ii=0;ii<0;ii++)
        // {
            
            timeHistToSpectrum(accTimeHist, tRsp.getXSeries(), params.dt, calSpec);
        // }
        
        isChecked=targetRspEnvCheck(tRsp, calSpec,logFile);
        std::cout<<isChecked<<std::endl;
        std::ofstream ofileS("Spectrum.txt", std::ios_base::out);
        for(int i=0;i<tRsp.getDataSize();i++)
        {
            double freq, valueT, valueC;
            freq=tRsp[i].getX();
            valueT=tRsp[i].getY();
            valueC=calSpec[i].getY();
            ofileS<<freq<<'\t'<<valueT<<'\t'<<valueC<<std::endl;
        }
        ofileS.close();
        // narrowBandAdjust(accTimeHist, tRsp, params, logFile);
        //timeHistAdjust(accTimeHist, tRsp, params, logFile);
        
        // std::ofstream ofileT("TimeHistory.txt", std::ios_base::out);
        // for(auto it=accTimeHist.begin();it!=accTimeHist.end();it++)
        // {
        //     double t;
        //     int loc;
        //     loc=it-accTimeHist.begin();
        //     t=loc*params.dt;
        //     ofileT<<t<<"\t"<<*it<<endl;
        // }
        // ofileT.close();
        //*******************************************************
        //计算反应谱与目标谱对比
    }
    logFile.close();

    return 0;
}