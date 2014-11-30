#include <stdio.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include "netcatLogger.h"


using namespace std;

class fileOperationsManager{

  public: 
	
	int writeToFile(const char* writeBuffer, const char *netcatPartDestFile);	
	char* readFromFile(const char *netcatPartInputFile, int seekToOffset, int readThisMuch,netcatLogger fileLogger);
};


