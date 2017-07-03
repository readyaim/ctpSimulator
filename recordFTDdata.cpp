// recordFTDdata.cpp : Defines the entry point for the console application.
#include <iostream>
#include "MdSpi.h"
#include "TdSpi.h"
#include "recordFTDdata.h"
#include "autoTradeSys.h"
#include <time.h>
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

extern char *Instrument_ID_Arr[]; 
extern int NUM_INS;

void pricing_trading(int _insIndex, CThostFtdcDepthMarketDataField *pDepthMarketData);

extern TdSpi *tdapi;

extern CTradeSysInsID tradeInsId[10];
extern int insIndex;


void cFTDKdata::init(TThostFtdcPriceType price){
	_high = price;
	_low = price;
	_open = price;
	_close = price;
}
void cFTDKdata::update(TThostFtdcPriceType price){
	_close = price;		//close
	if (price > _high) {
		_high = price;		//high
	}
	//low
	else if (price < _low) {
		_low = price;	//min
	}
}
void cFTDKdata::write2file(ofstream *pfile, char *PreUpdateTime, CThostFtdcDepthMarketDataField *pDepthMarketData){
	*pfile << pDepthMarketData->ActionDay << " "
		<< PreUpdateTime << " "
		<< _open << " "
		<< _close << " "
		<< _high << " "
		<< _low << endl;
}

void writeDayData2file(CThostFtdcDepthMarketDataField *pDepthMarketData){
	ofstream fday;
	char sfilename[60];
	sprintf(sfilename, "./%s_1d.txt", pDepthMarketData->InstrumentID);
	fday.open(sfilename, ios::app);
	if (fday.is_open())
	{
		fday << pDepthMarketData->ActionDay << " "
			<< pDepthMarketData->OpenPrice << " "
			<< pDepthMarketData->ClosePrice << " "
			<< pDepthMarketData->HighestPrice << " "
			<< pDepthMarketData->LowestPrice << " "
			<< pDepthMarketData->SettlementPrice << " "
			<< pDepthMarketData->PreSettlementPrice << " "
			<< endl;
		fday.close();
	}
	else
	{
		cout << "Can not open file" << sfilename << endl;
	}
}

void writeTickData2file(ofstream *pfile, CThostFtdcDepthMarketDataField *pDepthMarketData){
	*pfile << pDepthMarketData->ActionDay << " "
		<< pDepthMarketData->UpdateTime << " "
		<< pDepthMarketData->LastPrice << endl;
}

int recordFTDdata(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	int tmFTDhour[MAX_NUM_INS];
	int tmFTDmin[MAX_NUM_INS];
	int tmFTDsecond[MAX_NUM_INS];
	static int tmFlag[MAX_NUM_INS] = { 0 };
	char sfileNameMin[MAX_NUM_INS][60];
	static ofstream fpInsArry[MAX_NUM_INS];
	//generate InsIndex of MAX_NUM_INS
	int InsIndex = 0;	//index of instrument, max value is 20-1
	for (int i = 0; i <NUM_INS; i++){
		if (strcmp(Instrument_ID_Arr[i], pDepthMarketData->InstrumentID) == 0) {
			InsIndex = i;
			break;
		}
	}

	//change time from string to int
	if (EOF == sscanf(pDepthMarketData->UpdateTime, "%d:%d:%d", &tmFTDhour[InsIndex], &tmFTDmin[InsIndex], &tmFTDsecond[InsIndex])){
		//´íÎó
	}
	//20 futures maximum. stupid ways.
	static int tmStaticPreMin[MAX_NUM_INS] = { 0 };
	static cFTDKdata MinDataFTD[MAX_NUM_INS];
	static char PreUpdateTime[MAX_NUM_INS][9];

	//tmStaticPreMin¸³³õÖµ
	if (tmFlag[InsIndex] == 0) {
		tmStaticPreMin[InsIndex] = tmFTDmin[InsIndex];
		MinDataFTD[InsIndex].init(pDepthMarketData->LastPrice);//first run of .exe, init open/close/high/low
		strcpy(PreUpdateTime[InsIndex], pDepthMarketData->UpdateTime);
		sprintf(sfileNameMin[InsIndex], "./data/%s_1m_.txt", pDepthMarketData->InstrumentID);
		fpInsArry[InsIndex].open(sfileNameMin[InsIndex], ios::app);
		tmFlag[InsIndex] = 1;	//Clear Flag, so that tmStaticPreMin only init once.
	}
	//char sfilename[60];
	//sprintf(sfilename, "./%s_1m.txt", pDepthMarketData->InstrumentID);

#ifdef _COUT_ENABLE	
	cout << pDepthMarketData->UpdateTime << ":" << pDepthMarketData->InstrumentID << ",";
	cout << "InsIndex =" << InsIndex << ",";
	cout << "Inst.=" << Instrument_ID_Arr[InsIndex] << endl;
#endif

	//decide whether it's data for new minute.
	if (tmFTDmin[InsIndex] != tmStaticPreMin[InsIndex]) {
		//1. write data to file
		MinDataFTD[InsIndex].write2file(&fpInsArry[InsIndex], PreUpdateTime[InsIndex], pDepthMarketData);
		//fd.flush();
		//fpInsArry[InsIndex].close();			/* don't close static file pointer, reserved for next write*/
		//2. update tmStaticPreMin
		tmStaticPreMin[InsIndex] = tmFTDmin[InsIndex];
		//3.init MinDataFTD with the first data of new minute
		MinDataFTD[InsIndex].init(pDepthMarketData->LastPrice);//init open/close/high/low
	}
	// data for current minute, compare and save them.
	else {
		MinDataFTD[InsIndex].update(pDepthMarketData->LastPrice);
		strcpy(PreUpdateTime[InsIndex], pDepthMarketData->UpdateTime);
	}

	//write ftd day data to *_1d.txt
	//cout << "tmFTDhour[index]=" << tmFTDhour[InsIndex] << endl;
	if (tmFTDhour[InsIndex] >= 15 && tmFTDhour[InsIndex] < 16 && tmFTDmin[InsIndex] < 5)
	{
		cout << "output day data\n";
		writeDayData2file(pDepthMarketData);
	}
	//write every tick to files
#ifdef TICK_DATA_LOG_ENABLE
	static ofstream pfTickArray[MAX_NUM_INS];
	char sfilenameTick[MAX_NUM_INS][60];
	static int tickFlag[MAX_NUM_INS] = {0};
	//sprintf(sfilenameTick[InsIndex], "./%s_t.txt", pDepthMarketData->InstrumentID);
	if (tickFlag[InsIndex] == 0){
		sprintf(sfilenameTick[InsIndex], "./data/%s_t.txt", pDepthMarketData->InstrumentID);
		pfTickArray[InsIndex].open(sfilenameTick[InsIndex], ios::app);
		tickFlag[InsIndex] = 1;	//Clear Flag, so that tmStaticPreMin only init once.
	}
	writeTickData2file(&pfTickArray[InsIndex], pDepthMarketData);
	//fd.flush();
	//fd.close();
	//cout << GetTickCount();
#endif
	return 0;
}


