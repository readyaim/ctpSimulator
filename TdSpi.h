#include <iostream>
using namespace std;

#ifndef TDSPI_H
#define TDSPI_H

#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiStruct.h"
extern char *Instrument_ID_Arr[];
extern int flag;
#ifndef MAX_NUM_INS
#define MAX_NUM_INS 10
#endif
#define PARA_STOPHOP 15
//class TdSpi :public CThostFtdcTraderSpi{
class TdSpi{
public:
	//构造函数
	//TdSpi(CThostFtdcTraderApi *tdapi);
	TdSpi();
	//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询结算信息确认响应
	void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//请求查询投资者结算结果响应
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///用户口令更新请求响应
	void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询行情响应
	void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询投资者持仓响应
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///请求查询成交响应
	void OnRspQryTrade(CThostFtdcTradeField *pTrade, 
				CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//查询资金帐户响应
	void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
	
	void OnRtnOrder(CThostFtdcOrderField *pOrder);
	
	void OnRtnTrade(CThostFtdcTradeField *pTrade);
	 int ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, int nRequestID) ;
	 
	int ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID);

private:
	//CThostFtdcTraderApi *tdapi;
	CThostFtdcReqUserLoginField *loginField;
	CThostFtdcReqAuthenticateField *authField;
};


#endif