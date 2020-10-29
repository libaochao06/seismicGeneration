#include "seisGenUtils.h"
#include <algorithm>

void maxResp(const std::vector<double> &acc, double freq, double damp, double dt, std::vector<double> &results)
{
    double omega=PI2*freq;
    double ui, vi, uiNew, viNew, ag;
    int nSize=acc.size();
    ui=0;
    vi=0;
    double accOld, accNew;
    std::vector<double> accResp;
    double a1, a2, a3, a4;
    accOld=acc.front();
    accResp.push_back(accOld);
    for(int i=1;i<nSize;i++)
    {
        ag=acc.at(i);
        a1=1+damp*omega*dt+pow(omega*dt,2)/4;
        a2=damp*omega*dt+pow(omega*dt,2)/4;
        a3=2*damp*omega+pow(omega,2)*dt;
        a4=pow(omega,2);
        accNew=(-ag-a2*accOld-a3*vi-a4*ui)/a1;
        viNew=vi+dt/2*(accOld+accNew);
        uiNew=ui+vi*dt+pow(dt,2)/2*(accOld+accNew)/2;
        accResp.push_back(accNew+ag);
        accOld=accNew;
        ui=uiNew;
        vi=viNew;
    }
    int locMax=max_element(accResp.begin(), accResp.end())-accResp.begin();
    int locMin=min_element(accResp.begin(), accResp.end())-accResp.begin();

    if(accResp[locMax]>=fabs(accResp[locMin]))
    {
        results.push_back(locMax*dt);
        results.push_back(accResp[locMax]);
    }
    else
    {
        results.push_back(locMin*dt);
        results.push_back(accResp[locMin]);
    }
}