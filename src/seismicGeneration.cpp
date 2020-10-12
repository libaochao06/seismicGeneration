#include "seismicGeneration.h"
#include "seisGenUtils.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
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
    for(auto it=specs.begin();it!=specs.end();it++)
    {
        changeSpectrumUnit(*it, 1);
    }
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
    for(auto &tRsp : targetRsp)
    {
        logFile<<">>反应谱"<<tRsp.name<<"人工时时程拟合开始"<<endl;
        // 人工时程计算 Step:3.1 由目标反应谱计算近似功率密度谱
        logFile<<">>反应谱"<<tRsp.name<<"功率密度谱计算开始"<<endl;
    }

    logFile.close();

    return 0;
}