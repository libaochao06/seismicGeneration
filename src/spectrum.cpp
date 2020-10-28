#include <algorithm>
#include <iostream>
#include "spectrum.h"
#include <cmath>

Spectrum::Spectrum(spectrumXType xt, spectrumYType yt, float idamp, std::string iname)
{
    xType=xt;
    yType=yt;
    damp=idamp;
    name=iname;
}

Spectrum::Spectrum(const Spectrum &sp)
{
    name=sp.name;
    xType=sp.xType;
    yType=sp.yType;
    damp=sp.damp;
    data=sp.data;
}

Spectrum &Spectrum::operator=(const Spectrum &sp)
{
    data=sp.data;
    xType=sp.xType;
    yType=sp.yType;
    damp=sp.damp;
    name=sp.name;
    return *this;
}

void Spectrum::sortData(bool isAscendant)
{
    if(isAscendant)
    {
        sort(data.begin(), data.end(), xLessEqual);
    }
    else
    {
        sort(data.begin(), data.end(), xGreatEqual);
    }
    
}

double Spectrum::getValueByX(double x) const
{
    double value=0;
    vector<DataPoint> temp=data;
    sort(temp.begin(), temp.end(), xLessEqual);

    if(x<temp.front().getX() || x>temp.back().getX())
    {
        std::cout<<"函数 Spectrum::getValueByX 出现错误!\n";
        std::cout<<"输入X数值超出反应谱X轴数据范围!"<<std::endl;
        abort;
    }

    for(auto it=temp.begin(); it!=temp.end()-1; it++)
    {
        double x1, x2, y1, y2;
        x1=it->getX();
        x2=(it+1)->getX();
        if(x>=x1 && x<=x2)
        {
            y1=it->getY();
            y2=(it+1)->getY();
            if(fabs(x-x1)<1.0e-2)
                value=y1;
            else if(fabs(x-x2)<1.0e-2)
                value=y2;
            else
            {
                value=(log(y2) - log(y1)) / (log(x2) - log(x1))*(log(x) - log(x1)) + log(y1);
                value=exp(value);
            }
            break;
        }
    }

    return value;
}

vector<double> Spectrum::getXSeries() const
{
    vector<double> xSeries;
    for(auto it=data.begin(); it!=data.end(); it++)
    {
        xSeries.push_back(it->getX());
    }
    return xSeries;
}

vector<double> Spectrum::getYSeries() const
{
    vector<double> ySeries;
    for(auto it=data.begin(); it!=data.end(); it++)
    {
        ySeries.push_back(it->getY());
    }
    return ySeries;
}

DataPoint &Spectrum::operator[](int i)
{
    if(i>=0 && i<=data.size()-1)
        return data[i];
    else
        throw "Out of range!";
}

DataPoint Spectrum::operator[](int i) const
{
    if(i>=0 && i<=data.size()-1)
        return data[i];
    else
        throw "Out of range!";
}


void Spectrum::addData(DataPoint dp)
{
    data.push_back(dp);
}