#include "seisGenUtils.h"

void peakAdjust(std::vector<double>& acc, double amp)
{
    /**
     * @brief 调整人工时程的峰值加速度，使之等于目标峰值加速度
     * 仅在时程曲线极值处附近进行调整
     */
    //计算当前时程曲线峰值加速度
    double accMax=0;
    for(auto it=acc.begin(); it!=acc.end();it++)
    {
        accMax=fmax(accMax, fabs(*it));
    }
    //计算调整系数
    double ratio=amp/accMax;
    //
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        *it=(*it)*ratio;
    }
}