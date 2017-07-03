#include "MdSpi.h"
#include "recordFTDdata.h"
#include <iostream>
#include <cstring>
#include <fstream>
#ifndef __linux__
#include "windows.h"
#endif
using namespace std;

extern int flag;
extern const double stopHop_Arr[] = { 15, 15, 15, 15, 15,
15, 15, 15, 15, 15 };
//extern MdSpi *mdapi;

/*

extern const string BROKER;
extern const string USER_ID;
extern const string NULL_STR;

extern TThostFtdcFrontIDType gFrontID_cancelOrder;
extern TThostFtdcSessionIDType gSessionID_cancelOrder;
extern TThostFtdcOrderRefType gOrderRef_cancelOrder;
*/

//MdSpi::MdSpi(CThostFtdcMdApi *mdapi){
MdSpi::MdSpi(){
}

void MdSpi::OnFrontConnected(){
	cout << "MdSpi::OnFrontConnected\n" ;
	
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast){
	cout << "MdSpi::OnRspUserLogin\n";

}

void MdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast){

}

//Receive Response to Subscribed Market Data
void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	//cout << "Response to Subscribed Market Data is received"<< endl;
	cout << "Instrument ID: " << pSpecificInstrument->InstrumentID << "; " << pRspInfo->ErrorMsg << endl;
		//<< pRspInfo->ErrorID << ", " << pRspInfo->ErrorMsg << endl;
	//cout << "RepInfo ID is:" << pRspInfo->ErrorID << ", means:" << pRspInfo->ErrorMsg << endl;
}

//Receive Response to Un-subscribed Market Data
void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast){
}

void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	
#if (0)	

	//pricing
	static int trigger = 0;
	if ((flag == 1) || (trigger == 1)){
		pricing_system(pDepthMarketData);
		trigger = 1;
		flag = 0;
	}
#endif
/*    
	cout << "FTD return: "
		<< pDepthMarketData->ActionDay << ", "
		<< pDepthMarketData->InstrumentID << ", "
		<< pDepthMarketData->UpdateTime << ", "
		<< pDepthMarketData->LastPrice << endl;
*/
		//保存数据
        //recordFTDdata(pDepthMarketData);
#if (0)
	//开仓
	if (flag==1){
        static int flag_run_once=0;
            if (flag_run_once==0){
                cout<<"Tdspi is ready\n";
                flag_run_once=1;
                CThostFtdcInputOrderField *pInputOrder=new CThostFtdcInputOrderField();
				// THOST_FTDC_D_Buy, THOST_FTDC_D_Sell;
				//THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday

				InsertOrderInit(pInputOrder, 3100.0,THOST_FTDC_D_Buy, THOST_FTDC_OF_Open);	//BK
				//InsertOrderInit(pInputOrder, 2900.0,THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday);	//SP
				//InsertOrderInit(pInputOrder, 2900.0, THOST_FTDC_D_Sell, THOST_FTDC_OF_Open);		//SK
				//InsertOrderInit(pInputOrder, 3100.0, THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday);	//BP
                tdapi->ReqOrderInsert(pInputOrder, 10);
				delete pInputOrder;
            }
        }
#endif

#if (0)
		//撤单
	static int flag_run_once = 0;
		if (flag == 1){
			if (flag_run_once == 0){
				cout << "Tdspi is ready, cancel order\n";
				flag_run_once = 1;
				CThostFtdcInputOrderActionField *pInputOrderAction = new CThostFtdcInputOrderActionField();
				pInputOrderAction->FrontID = gFrontID_cancelOrder;
				pInputOrderAction->SessionID = gSessionID_cancelOrder;
				strcpy(pInputOrderAction->OrderRef, gOrderRef_cancelOrder);
				pInputOrderAction->ActionFlag = THOST_FTDC_AF_Delete;
				strcpy(pInputOrderAction->InstrumentID, "zn1709");
				/*cout << "Cancel order: FrontID=" << gFrontID_cancelOrder
					<< ",SessionID=" << gSessionID_cancelOrder
					<< ",OrderRef=" << gOrderRef_cancelOrder;*/

				//strcpy(pInputOrderAction->BrokerID, BROKER.c_str());
				//strcpy(pInputOrderAction->UserID, USER_ID.c_str());
				//strcpy(pInputOrderAction->OrderSysID, "128876");
				//strcpy(pInputOrderAction->ExchangeID, "SHFE");	//测试环境下全都是上期所
				tdapi->ReqOrderAction(pInputOrderAction, 10);
				cout << "run here, cancel order\n";
				delete pInputOrderAction;
			}
		}
#endif

}

