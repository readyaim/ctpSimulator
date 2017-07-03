#include <iostream>
#include "MdSpi.h"
#include "TdSpi.h"
#include "recordFTDdata.h"
#include "autoTradeSys.h"
#include "ThostFtdcUserApiStruct.h"
#include "table.h"
#include <time.h>
#ifndef __linux__
#include "windows.h"
#pragma warning( disable : 4996)	//use sprintf instead of sprintf_s
#else
#include <unistd.h>
#endif
#include <cstring>

using namespace std;

#define _DEBUG_PRICING_TRADING

extern char *Instrument_ID_Arr[];
extern int NUM_INS;
extern const double stopHop_Arr[];
//extern CThostFtdcMdApi *mdapi;

#ifdef CTP_ENABLE
extern CThostFtdcTraderApi *tdapi;
#else
extern TdSpi *tdapi;	//for ctpSimulator
#endif

extern const TThostFtdcTimeType MORNING_START_TIME = "09:01:00";
extern const TThostFtdcTimeType MORNING_SHUTOFF_TIME = "11:25:00";
extern const TThostFtdcTimeType AFTERNOON_START_TIME = "13:31:00";
extern const TThostFtdcTimeType AFTERNOON_SHUTOFF_TIME = "14:55:00";
extern const TThostFtdcTimeType NIGHT_START_TIME = "21:01:00";
extern const TThostFtdcTimeType NIGHT_SHUTOFF_TIME = "23:25:00";

bool isTradeTime = false;

//int MarketPosition = 0;
//int OrderSentNotDealed = 0;
//int OrderCancelSuccess = 0;
//int InsIndex=0;
CTradeSysInsID tradeInsId[MAX_NUM_INS];


extern const string NULL_STR;
extern const string USER_ID;
extern const string BROKER;

extern TThostFtdcFrontIDType gFrontID_cancelOrder;
extern TThostFtdcSessionIDType gSessionID_cancelOrder;
extern TThostFtdcOrderRefType gOrderRef_cancelOrder;

//Construction Function
CTradeSysInsID::CTradeSysInsID(){
	//开单
	_pInputOrder = new CThostFtdcInputOrderField();
	strcpy(_pInputOrder->BrokerID, BROKER.c_str());
	strcpy(_pInputOrder->InvestorID, USER_ID.c_str());
	_pInputOrder->VolumeTotalOriginal = 1;
	_pInputOrder->VolumeCondition = THOST_FTDC_VC_AV;	//???
	_pInputOrder->MinVolume = 1;
	_pInputOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
	//_pInputOrder->StopPrice = 3050;
	_pInputOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	_pInputOrder->IsAutoSuspend = 0;
	_pInputOrder->UserForceClose = 0;
	_pInputOrder->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	_pInputOrder->TimeCondition = THOST_FTDC_TC_GFD; //Valid Only Today
	//strcpy(_pInputOrder->OrderRef, "");	//系统分配,后续可以自己指定
	_pInputOrder->OrderPriceType = THOST_FTDC_OPT_LimitPrice; //THOST_FTDC_OPT_AnyPrice,THOST_FTDC_OPT_LimitPrice
	_sign = 1;
	_Direction = '\0';
	//撤单
    _pInputOrderAction = new CThostFtdcInputOrderActionField();
	_pInputOrderAction->FrontID = gFrontID_cancelOrder;
	_pInputOrderAction->ActionFlag = THOST_FTDC_AF_Delete;
	_pInputOrderAction->SessionID = gSessionID_cancelOrder;
	strcpy(_pInputOrderAction->OrderRef, gOrderRef_cancelOrder);

	//参数
	//_miniPoint = 5.0;	//最小变动单位
	//_stopHop = PARA_STOPHOP;	//止损跳动。止损变化=_StopHop*_MiniPoint
	//_marketPosition = 0;

}
CTradeSysInsID::~CTradeSysInsID(){
	//析构
    delete _pInputOrder;
    delete _pInputOrderAction;
}
void CTradeSysInsID::InsertOrderGen(TThostFtdcPriceType LimitPrice, TThostFtdcDirectionType Direction,
	char CombOffsetFlag_0, TThostFtdcPriceType stopPriceDiff, TThostFtdcRequestIDType RequestID){

	_pInputOrder->LimitPrice = LimitPrice;
	_pInputOrder->Direction = Direction;// THOST_FTDC_D_Buy, THOST_FTDC_D_Sell;
	if (Direction == THOST_FTDC_D_Buy) _sign = 1;
	else if (Direction == THOST_FTDC_D_Sell) _sign = -1;
	if (stopPriceDiff != 0) _pInputOrder->StopPrice = LimitPrice - _sign*stopPriceDiff;
	_pInputOrder->CombOffsetFlag[0] = CombOffsetFlag_0; //THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
	_pInputOrder->RequestID = RequestID;

	/*
	strcpy(_pInputOrder->BrokerID, BROKER.c_str());
	strcpy(_pInputOrder->InvestorID, USER_ID.c_str());
	strcpy(_pInputOrder->InstrumentID, _instrumentID);
	strcpy(_pInputOrder->OrderRef, "");
	_pInputOrder->Direction = Direction;// THOST_FTDC_D_Buy, THOST_FTDC_D_Sell;
	_pInputOrder->LimitPrice = LimitPrice;
	_pInputOrder->VolumeTotalOriginal = 1;
	_pInputOrder->VolumeCondition = THOST_FTDC_VC_AV;
	_pInputOrder->MinVolume = 1;
	_pInputOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
	_pInputOrder->StopPrice = 3050;
	_pInputOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	_pInputOrder->IsAutoSuspend = 0;
	_pInputOrder->RequestID = 100;
	_pInputOrder->UserForceClose = 0;
	_pInputOrder->CombOffsetFlag[0] = CombOffsetFlag_0; //THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
	_pInputOrder->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	*/
	/* Price Limited Order */
	_pInputOrder->OrderPriceType = THOST_FTDC_OPT_LimitPrice; //THOST_FTDC_OPT_AnyPrice,THOST_FTDC_OPT_LimitPrice
	//pInputOrder->LimitPrice = 3050.00; // place a price in double here
	_pInputOrder->TimeCondition = THOST_FTDC_TC_GFD; //Valid Only Today

}
void CTradeSysInsID::CancelOrderGen(){
}

inline void checkTradeTime(int &InsIndex, TThostFtdcTimeType &updateTime){
#ifndef CTP_ENABLE
		//if (strcmp(updateTime, DAY_SHUTOFF_TIME) == -1 || (strcmp(updateTime, EVENING_START_TIME) == 1)) {
		//	isTradeTime = true;
		//}
		//else {
		//	isTradeTime = false;
		//}
		isTradeTime = true;
#else
        
//        cout<<"InsIndex="<<InsIndex<<", NUM_INS-1="<<NUM_INS-1
//            <<"updatetime="<<updateTime<<endl;
//        cout<<strcmp(updateTime, NIGHT_SHUTOFF_TIME)<<endl;
//	if (InsIndex == NUM_INS - 1){
		if ((strcmp(updateTime, MORNING_START_TIME) >= 1 && (strcmp(updateTime, MORNING_SHUTOFF_TIME) <= -1)) ||
			(strcmp(updateTime, AFTERNOON_START_TIME) >= 1 && strcmp(updateTime, AFTERNOON_SHUTOFF_TIME) <= -1) ||
			(strcmp(updateTime, NIGHT_START_TIME) >= 1) && strcmp(updateTime, NIGHT_SHUTOFF_TIME) <= -1){
			isTradeTime = true;
		}
		else {
			isTradeTime = false;
		}
//	}
#endif
	
}

void InsertOrderInit(CThostFtdcInputOrderField *pInputOrder, TThostFtdcPriceType LimitPrice, TThostFtdcDirectionType Direction, char CombOffsetFlag_0){
	strcpy(pInputOrder->BrokerID, BROKER.c_str());
	strcpy(pInputOrder->InvestorID, USER_ID.c_str());
	strcpy(pInputOrder->InstrumentID, "zn1709");
	strcpy(pInputOrder->OrderRef, "");
	pInputOrder->Direction = Direction;// THOST_FTDC_D_Buy, THOST_FTDC_D_Sell;
	pInputOrder->LimitPrice = LimitPrice;
	pInputOrder->VolumeTotalOriginal = 1;
	pInputOrder->VolumeCondition = THOST_FTDC_VC_AV;
	pInputOrder->MinVolume = 1;
	pInputOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
	pInputOrder->StopPrice = 3050;
	pInputOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	pInputOrder->IsAutoSuspend = 0;
	pInputOrder->RequestID = 100;
	pInputOrder->UserForceClose = 0;
	pInputOrder->CombOffsetFlag[0] = CombOffsetFlag_0; //THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
	pInputOrder->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;

	/* Price Limited Order */
	pInputOrder->OrderPriceType = THOST_FTDC_OPT_LimitPrice; //THOST_FTDC_OPT_AnyPrice,THOST_FTDC_OPT_LimitPrice
	//pInputOrder->LimitPrice = 3050.00; // place a price in double here
	pInputOrder->TimeCondition = THOST_FTDC_TC_GFD; //Valid Only Today
}

//return index of table.h
int findIndex(char a[31]){
	char b[31];
	strcpy(b, a);
	b[2] = '\0';

	int index = -1;
	for (int i = 0; i < _TOTAL_FUTURES_NUM; i++){
		if (strcmp(b, FuturePara_Arr[i].InstrumentID) == 0){
			//index = i;
			return i;
			//break;
		}
	}
	//cout << index << endl;
	//if (index == -1){
	b[1] = '\0';
	//}
	for (int i = 0; i < _TOTAL_FUTURES_NUM; i++){
		if (strcmp(b, FuturePara_Arr[i].InstrumentID) == 0){
			//index = i;
			return i;
			//		break;
		}
	}
	if (index == -1){
		cout << "Error: can't find " << a << endl;
		getchar();
		return -1;
	}
	//return index;
}

inline void entryStrategy1(CThostFtdcDepthMarketDataField *pDepthMarketData, int InsIndex){
	//空仓。如满足开仓条件，则开仓，并进入下一个状态
	//B/S: THOST_FTDC_D_Buy, THOST_FTDC_D_Sell; K/P: THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
	if (isTradeTime){
		if (pDepthMarketData->AskPrice1 > pDepthMarketData->BidPrice1 + tradeInsId[InsIndex]._miniPoint) {
			cout << pDepthMarketData->UpdateTime << ":" << pDepthMarketData->InstrumentID << ", ";
			cout << "A/B=" << pDepthMarketData->AskPrice1 << "/" << pDepthMarketData->BidPrice1
				<< ",V=" << pDepthMarketData->AskVolume1 << "/" << pDepthMarketData->BidVolume1 << endl;
			if (pDepthMarketData->AskVolume1 < pDepthMarketData->BidVolume1 / 2){
				tradeInsId[InsIndex]._entryPrice = pDepthMarketData->BidPrice1 + tradeInsId[InsIndex]._miniPoint;
				tradeInsId[InsIndex]._exitPrice = pDepthMarketData->AskPrice1;
				tradeInsId[InsIndex]._Direction = THOST_FTDC_D_Buy;
				//可以省略一个Buy,Sell参数
				tradeInsId[InsIndex].InsertOrderGen(tradeInsId[InsIndex]._entryPrice, THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, 0, 10);
				//下单
				tdapi->ReqOrderInsert(tradeInsId[InsIndex]._pInputOrder, 10);
				tradeInsId[InsIndex]._stateMachine = 1;
#ifdef _DEBUG_PRICING_TRADING
				cout << pDepthMarketData->UpdateTime << ":";
				cout << pDepthMarketData->InstrumentID << ", BK at " << pDepthMarketData->BidPrice1 + tradeInsId[InsIndex]._miniPoint << ", ";
				cout << tradeInsId[InsIndex]._pInputOrder->InstrumentID << ","
					<< tradeInsId[InsIndex]._pInputOrder->Direction << ","
					<< tradeInsId[InsIndex]._pInputOrder->LimitPrice << ","
					<< tradeInsId[InsIndex]._pInputOrder->CombOffsetFlag[0] << ","
					<< tradeInsId[InsIndex]._pInputOrder->OrderPriceType << ","
					<< tradeInsId[InsIndex]._pInputOrder->OrderPriceType;
				cout << ",_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
			}
			else if ((pDepthMarketData->AskVolume1 / 2 > pDepthMarketData->BidVolume1)){
				tradeInsId[InsIndex]._entryPrice = pDepthMarketData->BidPrice1 + tradeInsId[InsIndex]._miniPoint;
				tradeInsId[InsIndex]._exitPrice = pDepthMarketData->BidPrice1;
				tradeInsId[InsIndex]._Direction = THOST_FTDC_D_Sell;
				tradeInsId[InsIndex].InsertOrderGen(tradeInsId[InsIndex]._entryPrice, THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, 0, 10);
				tdapi->ReqOrderInsert(tradeInsId[InsIndex]._pInputOrder, 10);
				tradeInsId[InsIndex]._stateMachine = 1;
#ifdef _DEBUG_PRICING_TRADING
				cout << pDepthMarketData->UpdateTime << ":";
				cout << pDepthMarketData->InstrumentID << ", SK at " << pDepthMarketData->BidPrice1 + tradeInsId[InsIndex]._miniPoint << ", ";
				cout << tradeInsId[InsIndex]._pInputOrder->InstrumentID << ","
					<< tradeInsId[InsIndex]._pInputOrder->Direction << ","
					<< tradeInsId[InsIndex]._pInputOrder->LimitPrice << ","
					<< tradeInsId[InsIndex]._pInputOrder->CombOffsetFlag[0] << ","
					<< tradeInsId[InsIndex]._pInputOrder->OrderPriceType << ","
					<< tradeInsId[InsIndex]._pInputOrder->OrderPriceType;
				cout << ",_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
			}
		}
	}
}
void pricing_trading(CThostFtdcDepthMarketDataField *pDepthMarketData){

	static int initFlag[MAX_NUM_INS] = { 0 };
	int InsIndex = 0;	//index of instrument, max value is 20-1
	for (int i = 0; i <NUM_INS; i++){
		if (strcmp(Instrument_ID_Arr[i], pDepthMarketData->InstrumentID) == 0) {
			InsIndex = i;
			break;
		}
	}
	if (initFlag[InsIndex] == 0){
		strcpy(tradeInsId[InsIndex]._instrumentID, pDepthMarketData->InstrumentID);
		strcpy(tradeInsId[InsIndex]._pInputOrderAction->InstrumentID, pDepthMarketData->InstrumentID);
		strcpy(tradeInsId[InsIndex]._pInputOrder->InstrumentID, pDepthMarketData->InstrumentID);
		initFlag[InsIndex] = 1;
		tradeInsId[InsIndex]._miniPoint = FuturePara_Arr[findIndex(pDepthMarketData->InstrumentID)].MiniPoint;
		tradeInsId[InsIndex]._stopHop = stopHop_Arr[InsIndex];
		tradeInsId[InsIndex]._marketPosition = 0;	//初始化为0，因为可能已经被OnRtnOrder改写
		tradeInsId[InsIndex]._stateMachine = 0;		//初始化为0，因为可能已经被OnRtnOrder改写
		tradeInsId[InsIndex]._exID = FuturePara_Arr[findIndex(pDepthMarketData->InstrumentID)].exID;
		tradeInsId[InsIndex]._stopStatus = 0;	
		cout << pDepthMarketData->UpdateTime  << ":";
		cout << "Trade Paras: " << tradeInsId[InsIndex]._instrumentID << ",_miniPoint=" << tradeInsId[InsIndex]._miniPoint
											<< ", _stopHop=" << tradeInsId[InsIndex]._stopHop 
											<< ", _marketPosition=" << tradeInsId[InsIndex]._marketPosition
											<< ", _state=" << tradeInsId[InsIndex]._stateMachine << endl;
	}
//    cout<<"isTradeTime="<<isTradeTime<<endl;
	switch (tradeInsId[InsIndex]._stateMachine) {
		case 0:
			entryStrategy1(pDepthMarketData, InsIndex);
			break;
		case 1:
			//开仓单未成交，撤单并等待
			if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Buy){
				if (pDepthMarketData->AskPrice1 >= tradeInsId[InsIndex]._entryPrice + tradeInsId[InsIndex]._stopHop * tradeInsId[InsIndex]._miniPoint){
					//1. 撤多单
					//当前ask价格>买入委托价格+5跳
					tdapi->ReqOrderAction(tradeInsId[InsIndex]._pInputOrderAction, 10);
					tradeInsId[InsIndex]._stateMachine = 2;
					tradeInsId[InsIndex]._cancelOrderTimer = 0;
#ifdef _DEBUG_PRICING_TRADING
					cout << pDepthMarketData->UpdateTime << ":";
					cout << tradeInsId[InsIndex]._pInputOrderAction->InstrumentID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->FrontID << ","
						<< tradeInsId[InsIndex]._pInputOrderAction->SessionID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->OrderRef << endl;
					cout <<  ", Cancel BK order, stop buy, set _state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
					
				}
			}
			else if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Sell){
				if (pDepthMarketData->BidPrice1 <= tradeInsId[InsIndex]._entryPrice - tradeInsId[InsIndex]._stopHop * tradeInsId[InsIndex]._miniPoint){
					//1. 撤空单
					//当前bid价格 < 委托价格-5跳
					tdapi->ReqOrderAction(tradeInsId[InsIndex]._pInputOrderAction, 10);
					tradeInsId[InsIndex]._stateMachine = 2;
					tradeInsId[InsIndex]._cancelOrderTimer = 0;
#ifdef _DEBUG_PRICING_TRADING
					cout << pDepthMarketData->UpdateTime << ":";
					cout << tradeInsId[InsIndex]._pInputOrderAction->InstrumentID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->FrontID << ","
						<< tradeInsId[InsIndex]._pInputOrderAction->SessionID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->OrderRef << endl;
					cout << ", Cancel SK order, stop buy, set _state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
				}
				break;
		case 2:
			//等待撤开仓单回报
			cout << pDepthMarketData->UpdateTime << ":";
			cout << pDepthMarketData->InstrumentID << "State=2, Wait for response of canceling BK/SK order\n";
			break;
		case 3:
			//等待平仓委托成交，if当前价格超价, then撤平仓单
			if (tradeInsId[InsIndex]._marketPosition == 1){
				if ((!isTradeTime && tradeInsId[InsIndex]._stopStatus == 0) || pDepthMarketData->BidPrice1 < tradeInsId[InsIndex]._exitPrice - tradeInsId[InsIndex]._stopHop * tradeInsId[InsIndex]._miniPoint){
					////SP委托，价格走低。当前bid价格 < SP委托价格-5跳, 则撤SP单
					tdapi->ReqOrderAction(tradeInsId[InsIndex]._pInputOrderAction, 10);
					tradeInsId[InsIndex]._stateMachine = 4;
					//撤单的4个必填选项
#ifdef _DEBUG_PRICING_TRADING
					cout << pDepthMarketData->UpdateTime << ":";
					cout << tradeInsId[InsIndex]._pInputOrderAction->InstrumentID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->FrontID << ","
						<< tradeInsId[InsIndex]._pInputOrderAction->SessionID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->OrderRef << endl;
					cout << pDepthMarketData->InstrumentID << ", Cancel SP order, failed to SP, set";
					cout << "_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
				}
			}
			else if (tradeInsId[InsIndex]._marketPosition == -1){
				//避免止损单与收盘平仓的isTradeTime冲突。收盘前对平仓单立即撤单->止损；对止损单不立即撤单，而是判断价格。
				if ((!isTradeTime && tradeInsId[InsIndex]._stopStatus==0) || pDepthMarketData->AskPrice1 > tradeInsId[InsIndex]._exitPrice + tradeInsId[InsIndex]._stopHop * tradeInsId[InsIndex]._miniPoint){
					////BP委托，价格走高。当前ask价格 > BP委托价格+5跳，则撤BP单
					tdapi->ReqOrderAction(tradeInsId[InsIndex]._pInputOrderAction, 10);
					tradeInsId[InsIndex]._stateMachine = 4;
					//撤单的4个必填选项
#ifdef _DEBUG_PRICING_TRADING
					cout << tradeInsId[InsIndex]._pInputOrderAction->InstrumentID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->FrontID << ","
						<< tradeInsId[InsIndex]._pInputOrderAction->SessionID << ", "
						<< tradeInsId[InsIndex]._pInputOrderAction->OrderRef << endl;
					cout << pDepthMarketData->UpdateTime  << ":";
					cout << pDepthMarketData->InstrumentID << ", Cancel BP order, failed to BP. Set";
					cout << "_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
				}
			}
			break;
		case 4:
			//等待撤平仓单回报
			cout << pDepthMarketData->UpdateTime  << ":";
			cout << pDepthMarketData->InstrumentID << "State=4, Wait for response of canceling  BP/SP order\n";
			break;
		case 5:
			//下立即成交止损单。用1档价位±1跳委托(另一种方法是停板价委托)。然后set State=4
			//B/S: THOST_FTDC_D_Buy, THOST_FTDC_D_Sell; K/P: THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
			if (tradeInsId[InsIndex]._marketPosition == 1){
				tradeInsId[InsIndex]._entryPrice = pDepthMarketData->BidPrice1 - tradeInsId[InsIndex]._miniPoint;
				//(tradeInsId[InsIndex]._stopHop-3)是指如果止损单没成交，则在价格继续走低(tradeInsId[InsIndex]._stopHop-3)后撤单，并重新止损。
				//预留2跳的缓冲
				tradeInsId[InsIndex]._exitPrice = pDepthMarketData->BidPrice1 + (tradeInsId[InsIndex]._stopHop - 2) * tradeInsId[InsIndex]._miniPoint;
				tradeInsId[InsIndex]._Direction = THOST_FTDC_D_Sell;
				//可以省略一个Buy,Sell参数
				tradeInsId[InsIndex].InsertOrderGen(tradeInsId[InsIndex]._entryPrice, tradeInsId[InsIndex]._Direction, THOST_FTDC_OF_CloseToday, 0, 10);
				tdapi->ReqOrderInsert(tradeInsId[InsIndex]._pInputOrder, 10);
				tradeInsId[InsIndex]._stateMachine = 3;
				tradeInsId[InsIndex]._stopStatus = 1;
#ifdef _DEBUG_PRICING_TRADING
				cout << pDepthMarketData->UpdateTime << ":";
				if (!isTradeTime) cout << "Not Trade Time, ";
				cout << pDepthMarketData->InstrumentID << ", Stop SP at " << pDepthMarketData->BidPrice1 - tradeInsId[InsIndex]._miniPoint;
				cout << ",_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif

			}
			else if (tradeInsId[InsIndex]._marketPosition == -1){
				tradeInsId[InsIndex]._entryPrice = pDepthMarketData->AskPrice1 + tradeInsId[InsIndex]._miniPoint;
				//(tradeInsId[InsIndex]._stopHop-3)是指如果止损单没成交，并且价格继续走高(tradeInsId[InsIndex]._stopHop-3)后撤单，并重新止损。
				//预留3跳的缓冲
				tradeInsId[InsIndex]._exitPrice = pDepthMarketData->AskPrice1 - (tradeInsId[InsIndex]._stopHop - 2) * tradeInsId[InsIndex]._miniPoint;
				tradeInsId[InsIndex]._Direction = THOST_FTDC_D_Buy;
				//可以省略一个Buy,Sell参数
				tradeInsId[InsIndex].InsertOrderGen(tradeInsId[InsIndex]._entryPrice, THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, 0, 10);
				tdapi->ReqOrderInsert(tradeInsId[InsIndex]._pInputOrder, 10);
				tradeInsId[InsIndex]._stateMachine = 3;
				tradeInsId[InsIndex]._stopStatus = 1;
#ifdef _DEBUG_PRICING_TRADING
				cout << pDepthMarketData->UpdateTime << ":";
				if (!isTradeTime) cout << "Not Trade Time, ";
				cout << pDepthMarketData->InstrumentID << ", Stop BP at " << pDepthMarketData->AskPrice1 + tradeInsId[InsIndex]._miniPoint;
				cout <<  ",_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
			}
			break;
		case 6:
			//省略平仓条件判断（如果有平仓条件判断需||!isTradeTime），直接下平仓单
			//B/S: THOST_FTDC_D_Buy, THOST_FTDC_D_Sell; K/P: THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
			//if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Buy){
			if (tradeInsId[InsIndex]._marketPosition == 1) {
				// THOST_FTDC_D_Buy, THOST_FTDC_D_Sell;
				//THOST_FTDC_OF_Close,THOST_FTDC_OF_Open,THOST_FTDC_OF_CloseToday
				if (tradeInsId[InsIndex]._exitPrice < 1) tradeInsId[InsIndex]._exitPrice = pDepthMarketData->BidPrice1;
				tradeInsId[InsIndex]._Direction = THOST_FTDC_D_Sell;
				//可以省略一个Buy,Sell参数
				tradeInsId[InsIndex].InsertOrderGen(tradeInsId[InsIndex]._exitPrice, tradeInsId[InsIndex]._Direction, THOST_FTDC_OF_CloseToday, 0, 10);
				tdapi->ReqOrderInsert(tradeInsId[InsIndex]._pInputOrder, 10);
				tradeInsId[InsIndex]._stateMachine = 3;
#ifdef _DEBUG_PRICING_TRADING
				cout << pDepthMarketData->UpdateTime << ":";
				cout << pDepthMarketData->InstrumentID << ", SP at " << tradeInsId[InsIndex]._exitPrice; 
				cout << ",_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif

			}
			//else if (tradeInsId[InsIndex]._Direction == THOST_FTDC_D_Sell){
			else if (tradeInsId[InsIndex]._marketPosition == -1) {
				if (tradeInsId[InsIndex]._exitPrice < 1) tradeInsId[InsIndex]._exitPrice = pDepthMarketData->AskPrice1;
				tradeInsId[InsIndex]._Direction = THOST_FTDC_D_Buy;
				//可以省略一个Buy,Sell参数
				tradeInsId[InsIndex].InsertOrderGen(tradeInsId[InsIndex]._exitPrice, tradeInsId[InsIndex]._Direction, THOST_FTDC_OF_CloseToday, 0, 10);
				tdapi->ReqOrderInsert(tradeInsId[InsIndex]._pInputOrder, 10);
				tradeInsId[InsIndex]._stateMachine = 3;
#ifdef _DEBUG_PRICING_TRADING
				cout << pDepthMarketData->UpdateTime << ":";
				cout << pDepthMarketData->InstrumentID << ", BP at " << tradeInsId[InsIndex]._exitPrice;
				cout << ",_state=" << tradeInsId[InsIndex]._stateMachine << endl;
#endif
			}
			break;
		
			//default:
			}
		}
	checkTradeTime(InsIndex, pDepthMarketData->UpdateTime);
}
