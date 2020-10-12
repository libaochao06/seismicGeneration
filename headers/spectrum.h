/**
 * @file spectrum.h
 * @brief 反应谱类头文件
 * @author BaochaoLI (baochao.li@simstar.com.cn)
 * @version 1.0
 * @date 2020-08-26
 * 
 * @copyright Copyright (c) 2020  原兴核技术（北京）有限公司
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2020-08-26 <td>1.0     <td>baochao LI     <td>初始定义
 * </table>
 */
#ifndef SPECTRUM_H_
#define SPECTRUM_H_

#include <vector>
#include "datapoint.h"
#include "globalDefines.h"
#include <string>

using std::vector;

class Spectrum
{
private:
    float damp;
    vector<DataPoint> data;
    spectrumXType xType;
    spectrumYType yType;
public:
    std::string name;
    Spectrum(spectrumXType xt, spectrumYType yt, float idamp, std::string iname="");
    Spectrum(const Spectrum &sp);
    Spectrum &operator=(const Spectrum & sp);
    ~Spectrum(){}
    spectrumXType getXType() const {return xType;}
    spectrumYType getYType() const {return yType;}
    float getDamp() const {return damp;}
    vector<DataPoint>::size_type getDataSize() const {return data.size();}
    void sortData(bool isAscendant=true);
    double getValueByX(double x) const;
    vector<double> getXSeries() const;
    vector<double> getYSeries() const;
    void addData(DataPoint dp);
    vector<DataPoint> & getData() {return data;}
    const vector<DataPoint> & getData() const {return data;}
    DataPoint &operator[](int i);
    DataPoint operator[](int i) const;
};

#endif