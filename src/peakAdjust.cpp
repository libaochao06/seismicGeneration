#include "seisGenUtils.h"

void peakAdjust(std::vector<double>& acc, double amp)
{
    double accMax=0;
    for(auto it=acc.begin(); it!=acc.end();it++)
    {
        accMax=fmax(accMax, fabs(*it));
    }
    double ratio=amp/accMax;
    for(auto it=acc.begin();it!=acc.end();it++)
    {
        *it=(*it)*ratio;
    }
}