objects=main.o recordFTDdata.o MdSpi.o TdSpi.o autoTradeSys.o ctpSimulator.o
run:$(objects)
	g++ -o run $(objects) -L./ -std=c++11
main.o: main.cpp  TdSpi.h MdSpi.h ThostFtdcUserApiStruct.h ThostFtdcUserApiDataType.h
	g++ -c main.cpp -std=c++11
MdSpi.o: MdSpi.cpp MdSpi.h  recordFTDdata.h 
	g++ -c MdSpi.cpp -std=c++11
TdSpi.o: TdSpi.cpp TdSpi.h recordFTDdata.h autoTradeSys.h
	g++ -c TdSpi.cpp -std=c++11
recordFTDdata.o: recordFTDdata.cpp recordFTDdata.h MdSpi.h autoTradeSys.h
	g++ -c recordFTDdata.cpp -std=c++11
autoTradeSys.o:autoTradeSys.cpp MdSpi.h TdSpi.h recordFTDdata.h autoTradeSys.h table.h ThostFtdcUserApiStruct.h
	g++ -c autoTradeSys.cpp -std=c++11
ctpSimulator.o:ctpSimulator.cpp MdSpi.h TdSpi.h recordFTDdata.h autoTradeSys.h ctpSimulator.h
	g++ -c ctpSimulator.cpp -std=c++11
.PHONY:clean
clean:
	rm run $(objects)
