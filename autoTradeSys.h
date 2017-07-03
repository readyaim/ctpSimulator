#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiStruct.h"
void InsertOrderInit(CThostFtdcInputOrderField *pInputOrder, TThostFtdcPriceType LimitPrice, TThostFtdcDirectionType Direction, char CombOffsetFlag_0);
void pricing_system(int insIndex, CThostFtdcDepthMarketDataField *pDepthMarketData);

#ifndef AUTOTRADESYSY_H
#define AUTOTRADESYSY_H
#define _TOTAL_FUTURES_NUM 51
typedef enum  {
	SHFE = 0,
	CZCE,
	DCE,
	FFEX
}t_ExchangeID;


typedef struct
{
	char InstrumentID[31];
	int BigPointValue;
	double MiniPoint;
	t_ExchangeID exID;

}t_FuturePara;

class CTradeSysInsID {
public:
	void InsertOrderGen(TThostFtdcPriceType LimitPrice, TThostFtdcDirectionType Direction, 
			char CombOffsetFlag_0, TThostFtdcPriceType stopPriceDiff, TThostFtdcRequestIDType RequestID);
	void CancelOrderGen();
	CTradeSysInsID();
	~CTradeSysInsID();

	TThostFtdcInstrumentIDType _instrumentID;
	//CThostFtdcInputOrderField _inputOrder;
	CThostFtdcInputOrderField *_pInputOrder;
	TThostFtdcPriceType _entryPrice;
	TThostFtdcPriceType _exitPrice;
	TThostFtdcPriceType _stopPrice;
	TThostFtdcPriceType _lastAskPrice;
	TThostFtdcPriceType _lastBidPrice;
	int _volume;
	int _marketPosition; //-1:????; 0???; 1:???
	TThostFtdcExchangeIDType  _exchangeID;	//SHFE,DCE,CZCE,FFEX
	t_ExchangeID _exID;
	int _stateMachine;
	int _stopStatus;		//����ֹ��������ƽ�ֵ�isTradeTime��ͻ������ǰ��ƽ�ֵ�������������ֹ�𣻶�ֹ�𵥲�����������
	int _nRequestID;
	TThostFtdcFrontIDType _frontID_cancelOrder;
	TThostFtdcSessionIDType _sessionID_cancelOrder;
	TThostFtdcOrderRefType _orderRef_cancelOrder;
	double _sign;
	TThostFtdcDirectionType _Direction;
	//CThostFtdcInputOrderActionField _InputOrderAction;
	CThostFtdcInputOrderActionField *_pInputOrderAction;
	
	double _miniPoint;	//��С������λ
	int _stopHop;	//ֹ����������
	
	int _cancelOrderTimer;	//��������Ӧ��ʱ
private:
};



#endif
