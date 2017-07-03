#include "autoTradeSys.h"

extern const t_FuturePara FuturePara_Arr[_TOTAL_FUTURES_NUM] = {
	//{InstrumentID, BigPointValue,MiniPoint}
	{ "rb", 10, 1, SHFE },	//SHFE
	{ "al", 5, 5, SHFE },
	{ "hc", 10, 1, SHFE },
	{ "bu", 10, 2, SHFE },
	{ "ni", 1, 10, SHFE },
	{ "sn", 1, 10, SHFE },
	{ "zn", 5, 5, SHFE },
	{ "ru", 10, 5, SHFE },
	{ "au", 1000, 0.5, SHFE },
	{ "cu", 5, 10, SHFE },
	{ "ag", 15, 1, SHFE },
	{ "fu", 50, 1, SHFE },
	{ "pb", 5, 5, SHFE },
	{ "wr", 10, 1, SHFE },
	{ "CF", 5, 5, CZCE },	//CZCE
	{ "MA", 10, 1, CZCE },
	{ "TA", 5, 2, CZCE },
	{ "ZC", 100, 0.2, CZCE },
	{ "FG", 20, 1, CZCE },
	{ "SR", 10, 1, CZCE },
	{ "JR", 20, 1, CZCE },
	{ "OI", 10, 2, CZCE },
	{ "PM", 50, 1, CZCE },
	{ "RI", 20, 1, CZCE },
	{ "RM", 10, 1, CZCE },
	{ "RS", 10, 1, CZCE },
	{ "SF", 5, 2, CZCE },
	{ "SM", 5, 2, CZCE },
	{ "WH", 20, 1, CZCE },
	{ "ZC", 100, 0.2, CZCE },
	{ "i", 100, 0.5, DCE },	//DCE
	{ "j", 100, 0.5, DCE },
	{ "jm", 60, 0.5, DCE },
	{ "l", 5, 5, DCE },
	{ "p", 10, 2, DCE },
	{ "v", 5, 5, DCE },
	{ "pp", 5, 1, DCE },
	{ "cs", 10, 1, DCE },
	{ "c", 10, 1, DCE },
	{ "m", 10, 1, DCE },
	{ "a", 10, 1, DCE },
	{ "b", 10, 1, DCE },
	{ "jd", 10, 1, DCE },
	{ "y", 10, 2, DCE },
	{ "bb", 500, 0.5, DCE },
	{ "fb", 500, 0.5, DCE },
	{ "IC", 300, 0.2, FFEX },	//FFEX
	{ "IF", 300, 0.2, FFEX },
	{ "IH", 300, 0.2, FFEX },
	{ "T", 10000, 0.005, FFEX },
	{ "TF", 10000, 0.005, FFEX }

};