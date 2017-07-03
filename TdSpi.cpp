#include "TdSpi.h"
#include "autoTradeSys.h"
#include "recordFTDdata.h"
#define USE_SIMULATION_ACCOUNT
#include <iostream>
using namespace std;
#include <cstring>

#include <fstream>

#ifndef __linux__
#include "windows.h"
#pragma warning( disable : 4996)	//use sprintf instead of sprintf_s
#endif


//Simulation Account and pwd
extern const string USER_ID = "093857";
extern const string PASS = "123456";
extern const string BROKER = "9999";

extern const string NULL_STR = "";
extern int MarketPosition;
extern int OrderSentNotDealed;

//int InsIndex=0;
extern CTradeSysInsID tradeInsId[10];


//Construction Function of class TdSpi
//TdSpi::TdSpi(CThostFtdcTraderApi *tdapi){
TdSpi::TdSpi(){
//	this->tdapi = tdapi;
}

//Connected to Front of CTP,(called before connetion is established)
void TdSpi::OnFrontConnected(){
	cout <<"TdSpi::OnFrontConnected\n";
}

//Response to User Login Request
void TdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "OnRspUserLogin\n";
	
	

}

//Crash...请求查询结算信息确认响应
void TdSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout<<"TdSpi::OnRspQrySettlementInfoConfirm\n";
	
}

//请求查询投资者结算结果响应
void TdSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "TdSpi::OnRspQrySettlementInfo\n";
	
}


//Response of Comfirmation for Settlement Information 投资者结算结果确认响应
void TdSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "TdSpi::OnRspSettlementInfoConfirm\n";
	

}

//Response to Query of Investor Position
void TdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "TdSpi::OnRspQryInvestorPosition\n";
}

//Response to Query of Money of Trading Account查询资金帐户响应
void TdSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "TdSpi::OnRspQryTradingAccount!\n";
	flag = 1;
}


///Response to Query of Trade is made or not?
void TdSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout<<"TdSpi::OnRspQryTrade\n";
}

///Response to Logout
void TdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
}

///Response of Password Update
void TdSpi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "OnRspUserPasswordUpdate is called\n" ;
	
}

///Response to Query of Market Data请求查询行情响应
void TdSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cout << "OnRspQryDepthMarketData" << endl;
	
}

//Direction=0,B; =1, S
//CombOffsetFlag[0]=0,K; =1,平昨，=3平今
//OrderStatus:a下单；3待成交；0成交；5撤单成功

void TdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) {
	cout << pOrder->InstrumentID<< ", Orderstatus=" << pOrder->OrderStatus << endl;;
	int InsIndex = 0;	//index of instrument, max value is 20-1
	for (int i = 0; i <NUM_INS; i++){
		if (strcmp(Instrument_ID_Arr[i], pOrder->InstrumentID) == 0) {
			InsIndex = i;
			break;
		}
	}
    if (pOrder->OrderStatus == '0' && tradeInsId[InsIndex]._stateMachine==1 ){
		//开仓单成交
		tradeInsId[InsIndex]._stateMachine=6;
		cout << pOrder->InstrumentID<< "_state=" << tradeInsId[InsIndex]._stateMachine << endl;
		////tradeInsId[InsIndex]._Direction or pOrder.Direction????????????
		if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Buy) {
			tradeInsId[InsIndex]._marketPosition=1;	//持多单
		}
		else if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Sell) {
			tradeInsId[InsIndex]._marketPosition=-1;	//持空单
		}
	}
	else if (pOrder->OrderStatus == '5' && tradeInsId[InsIndex]._stateMachine==2) {
		//开仓单撤单成功，重新判断开仓
		tradeInsId[InsIndex]._stateMachine=0;
		cout << pOrder->InstrumentID<< "_state=" << tradeInsId[InsIndex]._stateMachine << endl;
	}
	else if (pOrder->OrderStatus == '0' && tradeInsId[InsIndex]._stateMachine == 3) {
		//平仓单成交
		tradeInsId[InsIndex]._stateMachine=0;	
		tradeInsId[InsIndex]._marketPosition = 0;	//持仓清0
		cout << pOrder->InstrumentID << "_state=" << tradeInsId[InsIndex]._stateMachine << ", success!"<<endl;

	}
	else if (pOrder->OrderStatus == '5' && tradeInsId[InsIndex]._stateMachine == 4) {
		//平仓单撤单成功。在state5下平仓止损单
		tradeInsId[InsIndex]._stateMachine=5;
		//cout << "_state=" << tradeInsId[InsIndex]._stateMachine << endl;
	}
	else if (pOrder->OrderStatus == '3'){
		//保存撤单信息。需要针对不同品种赋值
	
		tradeInsId[InsIndex]._pInputOrderAction->FrontID = pOrder->FrontID;
		tradeInsId[InsIndex]._pInputOrderAction->SessionID = pOrder->SessionID;
		strcpy(tradeInsId[InsIndex]._pInputOrderAction->OrderRef, pOrder->OrderRef);
	}

	
	
#if (0)
	cout << "OrderStatus=" << pOrder->OrderStatus << ", "
		<< "PreState=" << OrderSentNotDealed << ",";
	
	
	//Save FrontID/SessionID/OrderRef if order is not dealed.
	if (pOrder->OrderStatus == '3'){
		gFrontID_cancelOrder = pOrder->FrontID;
		gSessionID_cancelOrder = pOrder->SessionID;
		strcpy(gOrderRef_cancelOrder,pOrder->OrderRef);
		OrderSentNotDealed = 1;	//重新连接后，初始化OrderSentNotDealed，表示有未成交报单。如要继续下单需要先撤单。
	}
	else if (pOrder->OrderStatus == '0'){
		gFrontID_cancelOrder = 0;
		gSessionID_cancelOrder = 0;
		strcpy(gOrderRef_cancelOrder,"");
		OrderSentNotDealed = 0;	//重新连接后，初始化OrderSentNotDealed，表示有未成交报单。如要继续下单需要先撤单。
	}
	else if (pOrder->OrderStatus == '5'){
		//撤单成功
		if (OrderSentNotDealed == 2) {
			//平仓撤单成功，可以下止损单；不能开仓
			OrderSentNotDealed = 3;	
		}
		else if (OrderSentNotDealed == 4){
			//开仓撤单成功，可以开仓
			OrderSentNotDealed = 0;
		}
		else if (OrderSentNotDealed==1){
			//断线后，通过其他方式撤单。此时进入初始状态.
			OrderSentNotDealed = 0;
		}
	}
	cout << "State=" << OrderSentNotDealed
		<< " ,MP(ORO)="<<MarketPosition<< endl;

#endif
}

void TdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade){
	cout<<"TdSpi::OnRtnTrade\n";
#if (0)
	cout << pTrade->InstrumentID << ", "
		<< pTrade->Price << ", "
		<< pTrade->Volume << ", ";

	static int trigger = 0;
	static int TotalVolume = 0;
	if ((flag == 1) || (trigger = 1)){
		if ((pTrade->Direction == '0') && (pTrade->OffsetFlag == '0')){
			//BK
			TotalVolume = TotalVolume + pTrade->Volume;
			//cout << pTrade->InstrumentID;
			cout << " dealed, BK" << "; ";
			if (MarketPosition != 0) cout << "Error, Duplicate Buy\n";
			MarketPosition = 1;
		}
		else if ((pTrade->Direction == '0') && (pTrade->OffsetFlag == '3')){
			//BP
			TotalVolume = TotalVolume + pTrade->Volume;
			//cout << pTrade->InstrumentID;
			cout << " dealed, BP" << "; ";
			if (MarketPosition == 0) cout << "Error, BP\n";
			//if (volume==0) MarketPosition = 0;
			//else cout << "Error, Buy more than 1\n";
			MarketPosition = 0;
		}
		else if ((pTrade->Direction == '1') && (pTrade->OffsetFlag == '0')){
			//SK
			TotalVolume = TotalVolume - pTrade->Volume;
			//cout << pTrade->InstrumentID;
			cout << " dealed, SK" << "; ";
			if (MarketPosition != 0) cout << "Error, Duplicate Sell\n";
			MarketPosition = -1;
		}
		else if ((pTrade->Direction == '1') && (pTrade->OffsetFlag == '3')){
			//SP
			TotalVolume = TotalVolume - pTrade->Volume;
			//cout << pTrade->InstrumentID;
			cout << " dealed, SP" << "; ";
			if (MarketPosition == 0) cout << "Error, SP\n";
			//if (volume==0 )MarketPosition = 0;
			//else cout << "Error, Sell more than 1\n";
			MarketPosition = 0;

		}
		cout << "tVol(ORT)="<<TotalVolume << endl;
		cout << "MP(ORT)=" << MarketPosition << "; ";
		//cout << "Volume=" << volume << endl;
		OrderSentNotDealed = 0;
		trigger = 1;//trigger on
	}
#endif
}

int TdSpi::ReqOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, int nRequestID){
	cout<<"ReqOrderAction,\n";
	return 0;
}
int TdSpi::ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder, int nRequestID) {
	cout<<"ReqOrderInsert\n";
	return 0;
}

