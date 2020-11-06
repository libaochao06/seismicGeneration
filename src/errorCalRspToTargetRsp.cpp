#include "seisGenUtils.h"

double errorCalRspToTargetRsp(const Spectrum &targetRsp, const Spectrum &calRsp)
{
    std::vector<double> freqPoints;
    freqPoints=targetRsp.getXSeries();
    double error=0;
    for(int i=0;i<freqPoints.size();i++)
    {
        double ratio;
        ratio=(targetRsp[i].getY()-calRsp[i].getY())/calRsp[i].getY();
        error+=pow(ratio,2);
    }
    error=sqrt(error/freqPoints.size());
    return error;
}