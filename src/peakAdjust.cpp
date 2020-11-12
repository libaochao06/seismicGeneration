#include "seisGenUtils.h"
#include <algorithm>

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
    if(ratio>1.3)
    {
        peakAdjust(acc, amp,0);
        return;
    }
    if(fabs(1.0-ratio)<0.01)
    {
        return;
    }
    //
    int numPeak;//时程曲线极值点调整个数
    numPeak=100;
    //数据点调整标识符
    //若已被调整过则为1，否则为0
    std::vector<int> indice;
    for(int i=0;i<acc.size();i++)
    {
        indice.push_back(0);
    }
    //查找极值绝对值最大的50个极值点位置
    std::vector<double> absAcc;//时程曲线绝对值
    std::vector<int> peakLoc;//极值点位置数组
    for(auto it=acc.cbegin();it!=acc.cend();it++)
    {
        absAcc.push_back(fabs(*it));
    }
    for(int i=0;i<numPeak;i++)
    {
        auto it=max_element(absAcc.begin(),absAcc.end());
        peakLoc.push_back(it-absAcc.begin());
        *it=0;
    }
    //对极值点进行调整，如果时程峰值加速度小于目标值，则对极值点进行放大调整
    if(accMax<amp)
    {
        for(int i=0;i<numPeak;i++)
        {
            int loc=peakLoc.at(i);
            if(indice.at(loc)==1)
                continue;
            int left,right;//极值点调整范围左右边界
            left=0;
            right=acc.size();
            //左界
            for(int j=loc;j>0;j--)
            {
                if(acc.at(j)*acc.at(j-1)<=0)
                {
                    left=j;
                    break;
                }
            }
            //右界
            for(int j=loc;j<acc.size()-1;j++)
            {
                if(acc.at(j)*acc.at(j+1)<=0)
                {   
                    right=j;
                    break;
                }
            }
            //在左右界内对时程曲线进行调整
            for(int j=left;j<right;j++)
            {
                acc.at(j)=acc.at(j)*ratio;
                indice.at(j)=1;
            }
        }
    }

    //检查时程曲线，如果存在超过目标峰值加速度的极值点，则对其进行调整
    for(int i=0;i<acc.size();i++)
    {
        if(fabs(acc.at(i))>amp)
        {
            int left, right;
            left=0;
            right=acc.size();
            for(int j=i;j>0;j--)
            {
                if(acc.at(j)*acc.at(j-1)<0)
                {
                    left=j;
                    break;
                }
            }
            for(int j=i;j<acc.size()-1;j++)
            {
                if(acc.at(j)*acc.at(j+1)<0)
                {
                    right=j;
                    break;
                }
            }

            for(int j=left;j<right;j++)
            {
                acc.at(j)=acc.at(j)*ratio;
            }
            // acc.at(i)=0.99*amp;
        }
    }
}

void peakAdjust(std::vector<double>& acc, double amp, int flag)
{
    if(flag!=0)
    {
        peakAdjust(acc, amp);
        return;
    }
    else
    {
        double accMax=0;
        for(auto it=acc.begin(); it!=acc.end();it++)
        {
            accMax=fmax(accMax, fabs(*it));
        }
        for(auto it=acc.begin();it!=acc.end();it++)
        {
            *it=(*it)*amp/accMax;
        }
    }
    
}