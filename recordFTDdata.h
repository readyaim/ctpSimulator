#ifndef _CTP_SIMULATOR_H
#define _CTP_SIMULATOR_H
#include "MdSpi.h"
#include "TdSpi.h"
class cFTDKdata {
public:
	void init(TThostFtdcPriceType price);
	void update(TThostFtdcPriceType price);
	void write2file(ofstream *pfile, char *PreUpdateTime, CThostFtdcDepthMarketDataField *pDepthMarketData);


private:
	TThostFtdcPriceType _high, _low, _open, _close;
	TThostFtdcPriceType _openDay, _closeDay, _highDay, _lowDay;
	TThostFtdcDateType _ActionDay;


};

#endif