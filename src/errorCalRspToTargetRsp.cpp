#include "seisGenUtils.h"

rspError errorCalRspToTargetRsp(const Spectrum &targetRsp, const Spectrum &calRsp)
{
    std::vector<double> freqPoints;
    freqPoints=targetRsp.getXSeries();
    double error=0;//误差的均方差
    double maxNegError=0, maxPosErr=0, maxErr=0;
    int nErr=0, nAbove=0, nBelow=0;
    for(int i=0;i<freqPoints.size();i++)
    {
        double ratio, calVal, tVal;
        calVal=calRsp[i].getY();
        tVal=targetRsp[i].getY();
        ratio=(tVal-calVal)/tVal;
        if(ratio<0)
        {
            nBelow++;
            maxNegError=fmin(ratio, maxNegError);
            if(ratio<-0.1)
                nErr++;
        }
        else if(ratio>0)
        {
            maxPosErr=fmax(maxPosErr, ratio);
            if(ratio>0.3)
                nAbove++;
        }
        error+=pow(ratio,2);
    }
    maxErr=fmax(fabs(maxNegError), maxPosErr);
    error=sqrt(error/freqPoints.size());

    rspError check(error, maxNegError, maxPosErr, maxErr, nErr, nBelow, nAbove);
    return check;
}