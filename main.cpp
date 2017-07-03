#include <iostream>
#include <fstream>
#include <string>
#include "TdSpi.h" 
#include "MdSpi.h"
#ifndef __linux__
	#include <windows.h>
	//mkdir folder
	#include <direct.h>
	#include "io.h"
#else
	#include <sys/stat.h>
	#include <sys/types.h>
    #include <unistd.h>
#endif
#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcUserApiDataType.h"

using namespace std;

void ftdgenerator();

int flag;

extern const string NULL_STR;
extern const string USER_ID;
extern const string BROKER;
TThostFtdcFrontIDType gFrontID_cancelOrder;
TThostFtdcSessionIDType gSessionID_cancelOrder;
TThostFtdcOrderRefType gOrderRef_cancelOrder;

//CThostFtdcMdApi *mdapi = CThostFtdcMdApi::CreateFtdcMdApi("./md_file/");
//MdSpi *mdspi = new MdSpi(mdapi);

void mkdir_func(){
#ifndef __linux__
	if (_access("./data", 0))	_mkdir("./data");
#else
	if (access("./data", 0))	mkdir("./data",0777);
#endif
}
//CThostFtdcTraderApi *tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi("./trader_file/");
//TdSpi *tdspi = new TdSpi(tdapi);
MdSpi *mdapi;
MdSpi myMdSpi;
TdSpi *tdapi;
TdSpi myTdSpi;
int main()
{
	tdapi=&myTdSpi;
	
	mdapi=&myMdSpi;
	mkdir_func();
	ftdgenerator();

//	char ch = getchar();
	return 0;

}
