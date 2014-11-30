#include <stdio.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include "netcatLogger.h"

using namespace std; 

/*****************************************************************************************************************************************************

	Method : netcatLogger()

	constructor to intialize the verbose state	
*****************************************************************************************************************************************************/
netcatLogger::netcatLogger(){
        verboseState = 0;
}

/*****************************************************************************************************************************************************

	Method : logMeOnVerbose
	
	Argument :
		logText : the text to be logged

	Displays the text on the screen if verbose option is enabled
*****************************************************************************************************************************************************/			
int netcatLogger::logMeOnVerbose(string logText){

        if(verboseState == 1 && !logText.empty()){
		cout << logText << endl;
        }
        return 0;
}
