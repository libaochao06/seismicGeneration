#include "datapoint.h"

DataPoint::DataPoint(double ix, double iy)
:x(ix),y(iy)
{
/**
 * @brief 构造函数
 */
}

DataPoint::DataPoint(const DataPoint &dp)
{/**
 * @brief 复制构造函数
 */
    x=dp.x;
    y=dp.y;
}

void DataPoint::setValue(double ix, double iy)
{
/**
 * @brief 设置数据
 */
    x=ix;
    y=iy;
}

void DataPoint::setX(double ix)
{
    x=ix;
}

void DataPoint::setY(double iy)
{
    y=iy;
}

bool DataPoint::operator==(const DataPoint &dp) const
{
/**
 * @brief 比较函数
 */
    if(x==dp.x && y==dp.y)
        return true;
    else
    {
        return false;
    }
    
}

double DataPoint::operator[](int i) const
{
    switch (i)
    {
    case 1:
        return x;
        break;
    case 2:
        return y;
        break;
    default:
        throw "out of range";
        break;
    }
}

bool xLessEqual(const DataPoint &dp1, const DataPoint &dp2)
{
/**
 * @brief 根据X值比较两组数据点，用于后续排序
 */
    return dp1.getX()<=dp2.getX();
}

bool xGreatEqual(const DataPoint &dp1, const DataPoint &dp2)
{
    return dp1.getX()>=dp2.getX();
}