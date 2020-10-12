/**
 * @file datapoint.h
 * @brief 键值数据对类
 * @author BaochaoLI (baochao.li@simstar.com.cn)
 * @version 1.0
 * @date 2020-08-26
 * 
 * @copyright Copyright (c) 2020  原兴核技术（北京）有限公司
 * 
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2020-08-26 <td>1.0     <td>Baochao LI     <td>初次生成
 * </table>
 */
#ifndef DATAPOINT_H_
#define DATAPOINT_H_

class DataPoint
{
private:
    double x;
    double y;
public:
    DataPoint(double ix, double iy);
    DataPoint(const DataPoint &dp);
    ~DataPoint(){};
    void setValue(double ix, double iy);
    double getX() const {return x;}
    void setX(double ix);
    double getY() const {return y;}
    void setY(double iy);
    bool operator==(const DataPoint &dp) const;
    double operator[](int i) const;
};

bool xLessEqual(const DataPoint &dp1, const DataPoint &dp2);
bool xGreatEqual(const DataPoint &dp1, const DataPoint &dp2);
#endif