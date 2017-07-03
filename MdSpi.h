#ifndef MDSPI_H
#define MDSPI_H
#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include <map>
#include <string>
#include <fstream>
using namespace std;
using std::map;
using std::string;
extern char *Instrument_ID_Arr[];
extern int NUM_INS;
class MdSpi{
public:
	//MdSpi(CThostFtdcMdApi *mdapi);
	MdSpi();
	//建立连接时触发
	void OnFrontConnected();
	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	///订阅行情应答
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	///取消订阅行情应答
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, 
		int nRequestID, bool bIsLast);
	///深度行情通知
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
private:
	//CThostFtdcMdApi *mdapi;
	CThostFtdcReqUserLoginField *loginField;
	int loginRequestID;
};

#endif