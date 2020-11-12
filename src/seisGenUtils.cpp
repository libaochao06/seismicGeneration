#include "seisGenUtils.h"
#include <numeric>
#include <algorithm>

using std::vector;
using std::ifstream;
using std::ofstream;
using std::string;
using std::istringstream;

void readInput(const std::string &infileName, SeisGenPara &params, std::vector<Spectrum> &specs, std::ofstream &logFile)
{
    logFile<<"*********************************"<<std::endl;
    logFile<<">>输入文件读取开始"<<std::endl;
    ifstream infileStream(infileName, std::ios_base::in);
    if(!infileStream.is_open())
    {
        logFile<<">>函数 readInput 无法打开输入文件 "<<infileName<<"！"<<std::endl;
    }
    //读入输入文件
    int lineCount=0;
    while(!infileStream.eof())
    {
        lineCount++;
        string line;
        std::getline(infileStream, line);
        if(line[0]=='*')
        {
            if(line[1]=='*')
                continue;
            else
            {
                vector<string> cmd;
                split(line, cmd, ',');
                //Parameter关键字处理
                if(cmd.front()=="*Parameter")
                {
                    //读取第一行并赋值
                    string pline;
                    std::getline(infileStream, pline);
                    lineCount++;
                    vector<string> values;
                    split(pline, values, ',');
                    params.dt=str2double(values[0]);
                    params.err=str2double(values[1]);
                    //读取第二行并赋值
                    std::getline(infileStream, pline);
                    lineCount++;
                    split(pline, values, ',');
                    params.tDuration=str2double(values[0]);
                    params.tRise=str2double(values[1]);
                    params.tDrop=str2double(values[2]);
                    //读取第三行
                    std::getline(infileStream, pline);
                    lineCount++;
                    trim(pline);
                    if(pline=="Vanmarcke")
                    {
                        params.flag=PSDFlag::Vanmarcke;
                    }
                    else if(pline=="RG160")
                    {
                        params.flag=PSDFlag::RG160;
                    }
                    else if(pline=="Kaul")
                    {
                        params.flag=PSDFlag::Kaul;
                    }
                    logFile<<">>全局计算参数读取完毕"<<std::endl;
                }
                //Spectrum关键字处理
                if(cmd.front()=="*Spectrum")
                {
                    vector<string> properties;
                    string name("New");
                    spectrumXType xType;
                    spectrumYType yType;
                    double maxAcc;
                    float damp;
                    for(auto it=cmd.begin()+1;it!=cmd.end();it++)
                    {
                        split(*it, properties, '=');
                        if(properties[0]=="name")
                        {
                            name=properties[1];
                        }
                        if(properties[0]=="xType")
                        {
                            if(properties[1]=="Freq")
                            {
                                xType=spectrumXType::Freq;
                            }
                            if(properties[1]=="Period")
                            {
                                xType=spectrumXType::Period;
                            }
                            if(properties[1]=="Omega")
                            {
                                xType=spectrumXType::Omega;
                            }
                        }
                        if(properties[0]=="yType")
                        {
                            if(properties[1]=="Accel")
                            {
                                yType=spectrumYType::Accel;
                            }
                            if(properties[1]=="Vel")
                            {
                                yType=spectrumYType::Vel;
                            }
                            if(properties[1]=="Disp")
                            {
                                yType=spectrumYType::Disp;
                            }
                        }
                        if(properties[0]=="maxAccel")
                        {
                            maxAcc=str2double(properties[1]);
                        }
                        if(properties[0]=="damp")
                        {
                            damp=str2double(properties[1]);
                        }
                    }
                    //根据参数创建反应谱对象
                    Spectrum spec(xType, yType, damp);
                    spec.name=name;
                    //向总体计算参数中插入该条反应谱的最大加速度参数
                    params.maxAccels.push_back(maxAcc);
                    //读取该条反应谱数据
                    while(1)
                    {
                        char next;
                        next=infileStream.peek();
                        if(next>='0' && next<='9')
                        {
                            vector<string> values;
                            string dataLine;
                            std::getline(infileStream, dataLine);
                            lineCount++;
                            split(dataLine, values, ',');
                            spec.addData(DataPoint(str2double(values[0]),str2double(values[1])));
                        }
                        else
                        {
                            break;
                        }
                        
                    }
                    //将该条反应谱插入到输出map中
                    specs.push_back(spec);
                    logFile<<">>反应谱"<<name<<"数据读取完毕，共读取"<<spec.getDataSize()<<"个反应谱数据点"<<std::endl;
                }
                //todo
            }    
        }
        else
        {
            logFile<<">>第"<<lineCount<<"行：\n"<<line<<" 不是合法输入，请查检！"<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    //输入文件读取完毕
    logFile<<">>输入文件读取完毕，共读取"<<specs.size()<<"组反应谱数据"<<std::endl;
    logFile<<"*********************************"<<std::endl;
    //关闭文件流
    infileStream.close();
}

void split(const string& s, vector<string>& sv, const char flag)
{
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        trim(temp, ' ');
        sv.push_back(temp);
    }
    return;
}

void trim(string &str, const char flag)
{
    if(str.empty())
    {
        return;
    }
    str.erase(0, str.find_first_not_of(flag));
    str.erase(str.find_last_not_of(flag)+1);
}

int str2int(const string &str)
{
    int num;
    std::stringstream ss(str);
    ss>>num;
    return num;
}

double str2double(const std::string &str)
{
    double num;
    std::stringstream ss(str);
    ss>>num;
    return num;
}

void changeSpectrumUnit(Spectrum &sp, int flag)
{
    if(flag==1)
    {
        for(int i=0;i<sp.getDataSize();i++)
        {
            sp[i].setY(sp[i].getY()*G);
        }
    }
    else if(flag==0)
    {
        for(int i=0;i<sp.getDataSize();i++)
        {
            sp[i].setY(sp[i].getY()/G);
        }
    }
    else
    {
        std::cerr<<"flag 数值错误"<<std::endl;
    }
    
}

int FFTParams(double tdur, double dt, double &Td, double &deltaFreq)
{
    int n=2;
    int nMin;
    nMin=ceil(tdur/dt);
    while(n<=nMin)
        n*=2;
    Td=(n-1)*dt;
    deltaFreq=1.0/Td;

    return n;
}

double rPeak(double probability, double Td, double freq, double damp)
{
/**
 * @brief 峰值系数计算函数
 */
    double rPeak, omega, xis, delta, coeffExp, twoN;
    omega=PI2*freq;
    twoN=2*freq*Td/(-log(probability));
    if(twoN<1)
        rPeak=1;
    else
    {
        xis=damp/(1-exp(-2*damp*omega*Td));//等效阻尼比
        delta=sqrt(4*xis/PI);//delta:带宽的一种度量
        coeffExp=-pow(delta, 1.2)*sqrt(PI*log(twoN));//指数系数
        rPeak=2*log(twoN*(1-exp(coeffExp)));//峰值系数的平方
        rPeak=fmax(1, rPeak);//峰值系数不能小于1
    }

    return sqrt(rPeak);
    
}

void accTimeHistIntegral(const std::vector<double> &acc, std::vector<double> &vel, std::vector<double> &disp, double dt)
{
    //由加速度时程积分求速度及位移时程曲线
    //v_(t+1)=v_t+(a_t+a_(t+1))*dt/2;
    //u_(t+1)=u_t+v_t*dt+(a_t/3+a_(t+1)/6)*dt^2
    std::vector<double>::size_type accSize=acc.size();
    vel.resize(accSize);
    disp.resize(accSize);
    
    vel[0]=0;
    disp[0]=0;
    
    for(int i=1;i<accSize;i++)
    {
        vel[i]=vel[i-1]+(acc[i-1]+acc[i])*dt/2;
        disp[i]=disp[i-1]+vel[i-1]*dt+(acc[i-1]/3+acc[i]/6)*pow(dt,2);
    }
}

void baselineAdjust(std::vector<double> &acc, double dt)
{
    //求加速度时程最大值，以备后面修正加速度峰值
    double accMax=0;
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        accMax=fmax(accMax, fabs(*it));
    }
    //速度、位移时程曲线临时数组
    std::vector<double> vel, disp;
    double Td=acc.size()*dt;//总时长
    double a0,a1;//修正系数
    double t;//当前时刻
    std::vector<double> ft;
    accTimeHistIntegral(acc, vel, disp, dt);
    for(int i=0;i<disp.size();i++)
    {
        double value=0;
        t=i*dt;
        value=disp[i]*(3*Td-2*t)*pow(t,2);
        ft.push_back(value);
    }
    double sum=0;
    sum=std::accumulate(ft.begin()+1,ft.end()-1,0.0);
    sum=sum+(ft.front()+ft.back())/2.0;
    sum=sum*dt;
    a1=28.0/13.0/pow(Td,2)*(2*vel.back()-15.0/pow(Td,5)*sum);
    a0=vel.back()/Td-a1*Td/2;
    //修正加速度曲线acc
    for(int i=0;i<acc.size();i++)
    {
        t=i*dt;
        acc.at(i)=acc.at(i)-(a0+a1*t);
    }
    //修正加速度峰值
    double newAccMax=0;
    for(auto it=acc.cbegin();it!=acc.cend();it++)
    {
        newAccMax=fmax(newAccMax, fabs(*it));
    }
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        *it=(*it)/newAccMax*accMax;
    }
}

void accEnvelop(std::vector<double> &acc, const std::vector<double> &envFunc)
{
    for(int i=0;i<acc.size();i++)
    {
        acc.at(i)=acc.at(i)*envFunc.at(i);
    }
}

void peakReduction(std::vector<double> &acc, double amp, const std::vector<double> &envFunc)
{
    for(int i=0;i<acc.size();i++)
    {
        double target;
        target=envFunc.at(i)*amp;
        if(fabs(acc.at(i))>target)
        {
            acc.at(i)=acc.at(i)/fabs(acc.at(i))*target;
        }
    }
}

double maxAbsOfTimeHist(const std::vector<double> &acc)
{
    double maxAbs=0;
    for(auto it=acc.cbegin();it!=acc.cend();it++)
    {
        maxAbs=fmax(maxAbs,fabs(*it));
    }
    return maxAbs;
}