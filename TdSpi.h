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
	//���캯��
	//TdSpi(CThostFtdcTraderApi *tdapi);
	TdSpi();
	//���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();

	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�����ѯ������Ϣȷ����Ӧ
	void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//�����ѯͶ���߽�������Ӧ
	void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//Ͷ���߽�����ȷ����Ӧ
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�û��������������Ӧ
	void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�����ѯ������Ӧ
	void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, 
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//�����ѯͶ���ֲ߳���Ӧ
	void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�����ѯ�ɽ���Ӧ
	void OnRspQryTrade(CThostFtdcTradeField *pTrade, 
				CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//��ѯ�ʽ��ʻ���Ӧ
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