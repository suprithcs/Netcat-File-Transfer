#include <stdio.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>

using namespace std;

class netcatLogger{

  public: 
	netcatLogger();
	int verboseState; 
	int logMeOnVerbose(string logText);
};

