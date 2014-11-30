
	
netcat_part :	fileManager.o	optionsManager.o netcatLogger.o	netcatPart.cpp	
		g++ -std=c++0x optionsManager.o fileManager.o netcatLogger.o netcatPart.cpp -lssl -lcrypto  -o netcat_part

fileManager.o:	fileOperationsManager.cpp
		g++ -std=c++0x -c fileOperationsManager.cpp -o fileManager.o

optionsManager.o:	optionsManager.cpp
		g++ -std=c++0x -c optionsManager.cpp -o optionsManager.o	

netcatLogger.o: netcatLogger.cpp
		g++ -std=c++0x -c netcatLogger.cpp -o netcatLogger.o

clean :
	rm *.o netcat_part















