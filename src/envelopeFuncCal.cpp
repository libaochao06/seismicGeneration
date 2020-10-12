#include "seisGenUtils.h"

void envelopeFuncCal(const double Td, const double tRise, const double tDrop, const double dt, const double nFour, std::vector<double> &envFunc)
{
    envFunc.clear();//清空数组
    double envConst, t, value;
    //包络曲线下降段系数c， 以保证地震结束时其加速度不大于强震的10%
    envConst=-log(0.1)/(Td-tDrop);
    for(int i=0;i<nFour;i++)
    {
        t=i*dt;
        if(t<=tRise)
            value=pow(t/tRise,2);//曲线上升段
        else if(t>tRise &&t<=tDrop)
        {
            value=1;//曲线平稳段
        }
        else if(t>tDrop && t<=Td)
        {
            value=exp(-envConst*(t-tDrop));//曲线下降段
        }
        else
        {
            value=0;
        }
        envFunc.push_back(value);
    }
}