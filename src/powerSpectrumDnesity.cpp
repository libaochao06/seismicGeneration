#include "powerSpectrumDensity.h"
#include <algorithm>

powerSpectrumDensity::powerSpectrumDensity(spectrumXType xt, spectrumYType yt, string pname)
{
    xType=xt;
    yType=yt;
    name=pname;
}

powerSpectrumDensity::powerSpectrumDensity(const powerSpectrumDensity &psd)
{
    xType=psd.xType;
    yType=psd.yType;
    name=psd.name;
    data=psd.data;
}

powerSpectrumDensity & powerSpectrumDensity::operator=(const powerSpectrumDensity &psd)
{
    xType=psd.xType;
    yType=psd.yType;
    name=psd.name;
    data=psd.data;
    return *this;
}

void powerSpectrumDensity::sortData(bool isAscendant)
{
    if(isAscendant)
    {
        std::sort(data.begin(), data.end(), xLessEqual);
    }
    else
    {
        std::sort(data.begin(), data.end(), xGreatEqual);
    }
}