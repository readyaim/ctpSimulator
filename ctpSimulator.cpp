#include <iostream>
#include "MdSpi.h"
#include "TdSpi.h"
#include "recordFTDdata.h"
#include "autoTradeSys.h"
#include "ctpSimulator.h"
#include <time.h>
extern const t_FuturePara FuturePara_Arr[_TOTAL_FUTURES_NUM];
#ifndef __linux__
#include "windows.h"
#pragma warning( disable : 4996)	//use sprintf instead of sprintf_s
#else
#include <unistd.h>
#endif
#include <cstring>
#ifndef MAX_NUM_INS
#define MAX_NUM_INS 10
#endif
using namespace std;
//#define _COUT_ENABLE	
#define TICK_DATA_LOG_ENABLE
#ifndef CTP_ENABLE
// write k-line data to file, start
char *Instrument_ID_Arr[] = {
	"rb1710", "cu1707", "FG709", "i1709", "zn1709","RM709", "au1712", "j1709"
};
int NUM_INS = 4;

//"rb1710", "cu1707", "FG709", "i1709", "RM709", "au1712", "j1709"
#else
extern char *Instrument_ID_Arr[];
extern int NUM_INS;
#endif

int recordFTDdata(CThostFtdcDepthMarketDataField *pDepthMarketData);
void pricing_trading(CThostFtdcDepthMarketDataField *pDepthMarketData);
int findIndex(char a[31]);
#ifdef CTP_ENABLE
extern CThostFtdcTraderApi *tdapi;
#else
extern TdSpi *tdapi;	//for ctpSimulator
#endif

extern CTradeSysInsID tradeInsId[10];

void OnRtnOrder_Gen(CThostFtdcDepthMarketDataField *pDepthMarketData){
	static int rtn_state[MAX_NUM_INS] ={0};
	static int rtn_dealed_state[MAX_NUM_INS]= {0};
	static int rtn_cancel_close_state[MAX_NUM_INS] = {0};
	static int rtn_close_state[MAX_NUM_INS] = {0};
	CThostFtdcOrderField *pOrder = new CThostFtdcOrderField();
	pOrder->OrderStatus = 'a';
	//撤开仓单回报
	int InsIndex = 0;	//index of instrument, max value is 20-1
	for (int i = 0; i <NUM_INS; i++){
		if (strcmp(Instrument_ID_Arr[i], pDepthMarketData->InstrumentID) == 0) {
			InsIndex = i;
			break;
		}
	}
    strcpy(pOrder->InstrumentID , pDepthMarketData->InstrumentID);
    if (tradeInsId[InsIndex]._stateMachine == 2){
		if (rtn_state[InsIndex] == 0) {
			pOrder->FrontID = 1;
			pOrder->SessionID = 8866886;
			strcpy(pOrder->OrderRef, "31");
			pOrder->OrderStatus = '3';	//待成交
			tdapi->OnRtnOrder(pOrder);
			rtn_state[InsIndex] = 1;
		}
		else if (rtn_state[InsIndex] == 1){
			pOrder->OrderStatus = '5';	//待成交
			tdapi->OnRtnOrder(pOrder);
			rtn_state[InsIndex] = 0;
		}
	}
	//撤平仓单回报
	if (tradeInsId[InsIndex]._stateMachine == 4){
		if (rtn_cancel_close_state[InsIndex] == 0) {
			pOrder->FrontID = 1;
			pOrder->SessionID = 8866889;
			strcpy(pOrder->OrderRef, "36");
			pOrder->OrderStatus = '3';	//待成交
			tdapi->OnRtnOrder(pOrder);
	rtn_cancel_close_state[InsIndex] = 1;
		}
		else if (rtn_cancel_close_state[InsIndex] == 1){
			pOrder->OrderStatus = '5';	//待成交
			tdapi->OnRtnOrder(pOrder);
			rtn_cancel_close_state[InsIndex] = 0;
		}
	}
	//开仓成交回报
	if (tradeInsId[InsIndex]._stateMachine == 1) {
		if (rtn_dealed_state[InsIndex] == 0){
			rtn_dealed_state[InsIndex] += 1;
			pOrder->FrontID = 1;
			pOrder->SessionID = 8866887;
			strcpy(pOrder->OrderRef, "32");
			pOrder->OrderStatus = '3';	//待成交
			tdapi->OnRtnOrder(pOrder);
		}
		else if (rtn_dealed_state[InsIndex] == 1){
			if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Buy){
				if (pDepthMarketData->LastPrice <= tradeInsId[InsIndex]._entryPrice){
					//BK成交
					pOrder->OrderStatus = '0';	//成交
					pOrder->Direction = THOST_FTDC_D_Buy;	//??????
					tdapi->OnRtnOrder(pOrder);
				}
			}
			else if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Sell){
				if (pDepthMarketData->LastPrice >= tradeInsId[InsIndex]._entryPrice){
					//SK成交
					pOrder->OrderStatus = '0';	//成交
					pOrder->Direction = THOST_FTDC_D_Sell;
					tdapi->OnRtnOrder(pOrder);
				}
			}
			rtn_dealed_state[InsIndex] = 0;
		}
	}
	//平仓成交回报
	//if (tradeInsId[InsIndex]._stateMachine == 6 || tradeInsId[InsIndex]._stateMachine==5) {
	if (tradeInsId[InsIndex]._stateMachine == 3) {
		if (rtn_close_state[InsIndex] == 0){
			rtn_close_state[InsIndex] += 1;
			pOrder->FrontID = 1;
			pOrder->SessionID = 8866888;
			strcpy(pOrder->OrderRef, "33");
			pOrder->OrderStatus = '3';	//待成交
			tdapi->OnRtnOrder(pOrder);
		}
		else if (rtn_close_state[InsIndex] == 1){
			if (tradeInsId[InsIndex]._Direction == '1'){
				if (pDepthMarketData->LastPrice >= tradeInsId[InsIndex]._exitPrice){
					//SP成交
					pOrder->OrderStatus = '0';	//成交
					tdapi->OnRtnOrder(pOrder);
				}
			}
			else if (tradeInsId[InsIndex]._Direction == '0'){
				if (pDepthMarketData->LastPrice <= tradeInsId[InsIndex]._exitPrice){
					//BP成交
					pOrder->OrderStatus = '0';	//成交
					tdapi->OnRtnOrder(pOrder);
				}
			}
			rtn_close_state[InsIndex] = 0;
		}
	}



}


void ftdgenerator() {
	//CThostFtdcDepthMarketDataField *pDepthMarketData, pMarketData;
	//pDepthMarketData = &pMarketData;
	CThostFtdcDepthMarketDataField *pDepthMarketData = new CThostFtdcDepthMarketDataField();
	int randomdata[MAX_NUM_INS] = {0};
	int baseprice[MAX_NUM_INS] = { 3000, 13500, 2500, 5000, 6000, 980, 15000 };
	int basepricelow[MAX_NUM_INS] = { 2980, 13180, 2380, 5000, 6000, 980, 15000 };
	int basepricehigh[MAX_NUM_INS] = { 3020, 13920, 2620, 5500, 7000, 1500, 18000 };
	//pDepthMarketData->LastPrice = 0.0;
	//strcpy(pDepthMarketData->TradingDay, "20170518");
	//strcpy(pDepthMarketData->ActionDay, "20170524");
	//strcpy(pDepthMarketData->InstrumentID, Instrument_ID_Arr[0]);
	//strcpy(pDepthMarketData->UpdateTime, "09:00:00");
	//setup seeds of rand() function
	int i[20] = { 0 };
	double signID[20] = { 0 };
	srand((int)time(0));
	while (true) {
		time_t tstamp = time(0);
		tm* newtime = localtime(&tstamp);
		sprintf(pDepthMarketData->UpdateTime, "%02d:%02d:%02d", newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
		sprintf(pDepthMarketData->ActionDay, "%04d%02d%02d", newtime->tm_year + 1900, newtime->tm_mon + 1, newtime->tm_mday);
		//	cout << newtime->tm_hour << ":"
		//		<< newtime->tm_min << ":"
		//		<< newtime->tm_sec << endl;
		for (int InsIndex = 0; InsIndex <NUM_INS; InsIndex++){
			//price
            
				strcpy(pDepthMarketData->InstrumentID, Instrument_ID_Arr[InsIndex]);
				//int m;
				//m = rand() % 3;
				//cout << m << endl;
				randomdata[InsIndex] = rand() % 3;
				
				baseprice[InsIndex] = baseprice[InsIndex] + (signID[InsIndex]+1) * randomdata[InsIndex];
                		//cout<<pDepthMarketData->InstrumentID<<","<<(signID[InsIndex]+1)*randomdata[InsIndex]<<endl;
				pDepthMarketData->LastPrice = baseprice[InsIndex] * FuturePara_Arr[findIndex(pDepthMarketData->InstrumentID)].MiniPoint;
				//cout << pDepthMarketData->LastPrice << endl;
				pDepthMarketData->AskPrice1 = ( baseprice[InsIndex] +1+ (rand() % 2))* FuturePara_Arr[findIndex(pDepthMarketData->InstrumentID)].MiniPoint;
				pDepthMarketData->BidPrice1 = baseprice[InsIndex] * FuturePara_Arr[findIndex(pDepthMarketData->InstrumentID)].MiniPoint;
				if (signID[InsIndex] == 1) pDepthMarketData->LastPrice = pDepthMarketData->AskPrice1;
				if (signID[InsIndex] == -1) pDepthMarketData->LastPrice = pDepthMarketData->BidPrice1;

				//pDepthMarketData->AskPrice2 = pDepthMarketData->AskPrice1 + 1 *  tradeInsId[InsIndex]._miniPoint;
				//pDepthMarketData->BidPrice2 = pDepthMarketData->BidPrice1 - 1 *  tradeInsId[InsIndex]._miniPoint;
				pDepthMarketData->AskVolume1 = rand() % 20 + 5;
				pDepthMarketData->BidVolume1 = rand() % 20 + 5;


			//cout << pDepthMarketData->InstrumentID<<"," << pDepthMarketData->LastPrice << ", "
			//	<<"Ask Price:"<< pDepthMarketData->AskPrice1<<", "
			//	<<"Bid Price:"<< pDepthMarketData->BidPrice1<<", "
			//	<<"Ask Volume:"<< pDepthMarketData->AskVolume1<<", "
			//	<<"Bid Volume:"<< pDepthMarketData->BidVolume1<<endl;

			if (baseprice[InsIndex] > basepricehigh[InsIndex]) {
				signID[InsIndex] = -2;	//避免给signID赋初值1
				//baseprice[InsIndex] += (rand()%3-1);
			}
			else if (baseprice[InsIndex]<basepricelow[InsIndex]){
				signID[InsIndex] = 0;
			}

			recordFTDdata(pDepthMarketData);
			pricing_trading(pDepthMarketData);
			OnRtnOrder_Gen(pDepthMarketData);

		}
#ifdef __linux__
		usleep(500000);
#else
		Sleep(500);
#endif

	}

}
