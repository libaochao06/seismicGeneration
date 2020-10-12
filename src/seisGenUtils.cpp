#include "seisGenUtils.h"

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