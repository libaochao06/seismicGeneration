#include "seisGenUtils.h"

bool targetRspEnvCheck(const Spectrum &targetRsp, const Spectrum &calRsp, std::ofstream &logFile)
{
    /**
	 * @函数说明: 计算反应谱与目标谱之间的误差
	 * @版本: 1.0
	 * @param {type} {
	 *     calRsp:计算反应谱
	 *     targetRsp:目标反应谱
	 *        }
	 * @返回值: bool，符合规范要求为true，否则为false
	 * @详细说明: 法规要求：1.计算反应谱值低于目标反应谱的点数不能超过5个
	 *						2.计算反应谱值低于目标反应谱最大不能超过10%
	 *						3.计算反应谱值高于目标反应谱超过30%的点数不能超过5个
	 */
    int nSize;
    nSize=targetRsp.getDataSize();
    double maxErr=0, maxPosErr=0, maxNegErr=0;//最大误差，最大正误差，最大负误差
    int nErr=0, nAbove=0, nBelow=0;//低于目标谱10%控制点个数， 高于目标谱30%控制点个数，低于目标谱控制点个数
    //std::cout<<calRsp.getData().at(121).getY()<<' '<<targetRsp.getData().at(121).getY()<<std::endl;
    for(int i=0;i<nSize;i++)
    {
        double calVal, tVal;
        calVal=calRsp.getData().at(i).getY();
        tVal=targetRsp.getData().at(i).getY();
        double diff=0;
        diff=(calVal-tVal)/tVal;
        if(diff<0)
        {
            nBelow++;
            maxNegErr=fmin(diff, maxNegErr);
            if(diff<-0.1)
                nErr++;
        }
        else if(diff>0)
        {
            maxPosErr=fmax(maxPosErr, diff);
            if(diff>0.3)
                nAbove++;
        }
    }
    maxErr = fmax(fabs(maxNegErr), maxPosErr);
    //logFile << "hello"<<std::endl;
    logFile << "计算反应谱与目标反应谱包络性检查结果："<<std::endl;
	logFile << "最大误差：" << maxErr <<"；最大正误差："<<maxPosErr<<"；最大负误差："<<maxNegErr<< '\n';
	logFile << "低于目标谱10%控制点个数：" << nErr << '\n';
	logFile << "低于目标谱控制点个数：" << nBelow << '\n';
	logFile << "高于目标谱30%控制点个数：" << nAbove << std::endl;
    if (nErr > 0)
		return false;
	if (nBelow > 5)
		return false;
	if (nAbove > 5)
		return false;
	return true;
}

bool targetRspEnvCheck(const rspError &err, std::ofstream &logFile)
{
    logFile << "计算反应谱与目标反应谱包络性检查结果："<<std::endl;
	logFile << "最大误差：" << err.maxErr <<"；最大正误差："<<err.maxPosErr<<"；最大负误差："<<err.maxNegErr<< '\n';
	logFile << "低于目标谱10%控制点个数：" << err.nErr << '\n';
	logFile << "低于目标谱控制点个数：" << err.nBelow << '\n';
	logFile << "高于目标谱30%控制点个数：" << err.nAbove << std::endl;
    if (err.nErr > 0)
		return false;
	if (err.nBelow > 5)
		return false;
	if (err.nAbove > 5)
		return false;
	return true;
}