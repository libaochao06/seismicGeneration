/**
 * @file seisGenPara.h
 * @brief 人工时程基本计算参数类头文件
 * @author BaochaoLI (baochao.li@simstar.com.cn)
 * @version 1.0
 * @date 2020-08-28
 * 
 * @copyright Copyright (c) 2020  原兴核技术（北京）有限公司
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2020-08-28 <td>1.0     <td>Baochao LI     <td>初次创建
 * </table>
 */
#ifndef SEISGENPARA_H_
#define SEISGENPARA_H_

#include "globalDefines.h"
#include <vector>
#include <string>

class SeisGenPara
{
public:
    double dt;
    double err;
    double tDuration, tRise, tDrop;
    PSDFlag flag;
    std::vector<double> maxAccels;
    SeisGenPara(double idt, double ierr, double itDur, double itRise, double itDrop, PSDFlag iflag);
    SeisGenPara(){};
    ~SeisGenPara(){};
};

#endif