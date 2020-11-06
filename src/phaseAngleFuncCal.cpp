#include "seisGenUtils.h"
#include <random>
#include <ctime>
#include <cmath>
#include <cfloat>
#include <numeric>

void phaseAngleFuncCal(const SeisGenPara &params, int nFour, std::vector<double> &phaseAngleFunc, phaseAngleCalMethod method)
{
    /**
     * @brief 人工时程相位角计算
     * 方法Random：相位角在0-2pi范围内随机分布
     * 方法EnvFunc：按照包络曲线形状计算相位差分，再由相位差分计算相位角
     */
    phaseAngleFunc.clear();
    switch (method)
    {
    case phaseAngleCalMethod::Random:
        {
            std::default_random_engine generator(time(NULL));
            std::uniform_real_distribution<double> distr(0,std::nextafter(PI2, DBL_MAX));
            for(int i=0;i<nFour/2-1;i++)
            {
                phaseAngleFunc.push_back(distr(generator));
            }
            break;
        }
    case phaseAngleCalMethod::EnvFunc:
        {
            double areaEnvFunc=0;//包络曲线下面积
            int N;//相位差个数
            vector<double> phaseAngleDiff, envFunc;//相位差分数组及计算用包络曲线
            //计算用包络曲线数据点个数128-256基本上够用
            // envelopeFuncCal(params.dt*(nFour-1), params.tRise, params.tDrop, params.dt, 256, envFunc);
            envelopeFuncCal(params.tDuration, params.tRise, params.tDrop, params.dt, 256, envFunc);
            vector<double> acculProbalDenFunc;//包络曲线的累积概率密度函数
            N=nFour/2-2;
            phaseAngleFunc.resize(N+1);
            phaseAngleDiff.resize(N);
            //计算包络曲线下面积
            areaEnvFunc=std::accumulate(envFunc.begin(), envFunc.end(), 0.0);
            double acculProba=0;
            for(auto it=envFunc.begin();it!=envFunc.end();it++)
            {
                acculProba=std::accumulate(envFunc.begin(),it+1, 0.0)/areaEnvFunc;
                acculProbalDenFunc.push_back(acculProba);
            }
            //相位差分计算
            srand(std::time(NULL));
            for(int i=0;i<N;i++)
            {
                double random;
                int nAcc=999;
                random=rand()%(nAcc+1)/(float)(nAcc+1);
                for(auto it=acculProbalDenFunc.begin()+1;it!=acculProbalDenFunc.end();it++)
                {
                    if(random<=*it)
                    {
                        double dy=*it-*(it-1);
                        double dx=1.0/(envFunc.size()-1);
                        double phidiff;
                        int loc=it-1-acculProbalDenFunc.begin();
                        phidiff=(loc+(random-*(it-1))/dy)*PI2*dx;
                        phaseAngleDiff[i]=phidiff;
                        break;
                    }
                }
            }
            //相位角计算
            phaseAngleFunc[0]=0;
            for(int i=0;i<N;i++)
            {
                phaseAngleFunc[i+1]=fmod(phaseAngleFunc[i]+phaseAngleDiff[i],PI2);
            }
            break;
        }
    default:
        break;
    }
}