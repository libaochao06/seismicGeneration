#include "seisGenUtils.h"

void respConvolution(const std::vector<double> &f, const std::vector<double> &g, std::vector<double> &out, const SeisGenPara &params)
{
    int nSize;
    nSize=f.size();
    double dt;
    dt=params.dt;
    out.clear();

    for(int i=0;i<nSize;i++)
    {
        double t;
        t=i*dt;
        double value=0;;
        for(int j=0;j<=i;j++)
        {
            double vf, vg, vp;
            vf=f.at(j);
            vg=g.at(i-j);
            vp=(vf+vg)*dt;
            value+=vp;
        }
        out.push_back(value);
    }
}