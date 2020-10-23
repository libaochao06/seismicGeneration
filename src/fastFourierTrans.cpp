#include "seisGenUtils.h"

void fastFourierTrans(const std::vector<std::complex<double>> &in, std::vector<std::complex<double>> &out, int inv)
{
/**
 * @brief 快速傅里叶变换程序 Xm=Sigma(Ck*epx(i*omega*t))
 * in: 输入复数数据数组
 * out：输出复数数据数组
 * inv：标识符 -1为快速傅里叶变换，由点值求系数
 *             1为快速傅里叶逆变换，由系数求点值
 */

    typedef std::complex<double> cp;
    int nSize=in.size();
    out.resize(nSize);
    //Fourier变换数据点个数N=2^order
    int order=log(nSize)/log(2);
    //创建按位逆序数组
    int *rev=new int[nSize];
    rev[0]=0;
    //计算逆序数组
    for(int i=1;i<nSize;i++)
    {
        rev[i]=(rev[i>>1]>>1)|((i&1)<<order-1);
    }
    //将原始数据按照逆序赋值给out
    for(int i=0;i<nSize;i++)
    {
        out[rev[i]]=in[i];
    }
    //释放逆序数组内存
    delete [] rev;
    //迭代法进行Fourier变换
    for(int mid=1; mid<nSize;mid*=2)
    {
        cp omegaN(cos(PI/mid), inv*sin(PI/mid));
        for(int i=0;i<nSize;i+=mid*2)
        {
            cp omega(1,0);
            for(int j=0;j<mid;j++)
            {
                cp t=omega*out[i+j+mid];
                cp u=out[i+j];
                out[i+j]=u+t;
                out[i+j+mid]=u-t;
                omega=omega*omegaN;
            }
        }
    }
    if(inv==-1)
    {
        for(auto it=out.begin();it!=out.end();it++)
        {
            it->real(it->real()/nSize);
            it->imag(it->imag()/nSize);
        }
    }
}