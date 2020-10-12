/**
 * @file powerSpectrumDensity.h
 * @brief 功率密度谱头文件定义
 * @author BaochaoLI (baochao.li@simstar.com.cn)
 * @version 1.0
 * @date 2020-10-12
 * 
 * @copyright Copyright (c) 2020  原兴核技术（北京）有限公司
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2020-10-12 <td>1.0     <td>Baochao LI     <td>内容
 * </table>
 */
#ifndef POWERSPECTRUMDENSITY_H_
#define POWERSPECTRUMDENSITY_H_

#include "globalDefines.h"
#include "datapoint.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class powerSpectrumDensity
{
private:
    spectrumXType xType;
    spectrumYType yType;
public:
    string name;
    vector<DataPoint> data;
    powerSpectrumDensity(spectrumXType xt, spectrumYType yt, string pname="");
    powerSpectrumDensity(const powerSpectrumDensity &psd);
    powerSpectrumDensity &operator=(const powerSpectrumDensity &psd);
    spectrumXType getXType() const {return xType;}
    spectrumYType getYType() const {return yType;}
    void setXType(spectrumXType xt) {xType=xt;}
    void setYType(spectrumYType yt) {yType=yt;}
    ~powerSpectrumDensity(){};
    void sortData(bool isAscendant=true);
};

#endif