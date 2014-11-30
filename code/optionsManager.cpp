#include <iostream>
#include <string.h>
#include <unistd.h>
#include <map>
#include "optionsManager.h"

using namespace std;


/*****************************************************************************************************************************************************

	Method : readOptions

	argc: No of arguments from the command line
	argv : All command line arguments are stored as an array
*****************************************************************************************************************************************************/

void optionsManager::readOptions(int argc,char** argv) {

	char ch;
	while ((ch = getopt(argc, argv, "lm:hvp:n:o:")) != -1) {
		
	    switch (ch) {
		
	    case 'h': //help
		optionDict.insert(pair<string,string>{"help", GetStandardUsageOptionScreen()});
		return;
	      break;
	    case 'l': //listen
		optionDict.insert(pair<string,string>{"server","true"});		
	      break;
	    case 'p': //port
		optionDict.insert(pair<string,string>{"port",optarg});
	      break;
	    case 'o'://offset
	      	optionDict.insert(pair<string,string>{"offset",optarg});
	      break;
	    case 'n'://bytes
		optionDict.insert(pair<string,string>{"nBytes",optarg}); 
	      break;
	    case 'v':
	      	optionDict.insert(pair<string,string>{"verbose","true"}); 
	      break;
	    case 'm':
		//Assign dynamic memory if you can	
	      optionDict.insert(pair<string,string>("message",optarg));
	      break;
	    default:
		//fprintf(stderr,"ERROR: Unknown option '-%c'\n",ch);
		//fprintf(stderr,GetStandardUsageOptionScreen());
		//optionDict.insert(pair<string,string>{"error",GetStandardUsageOptionScreen()});	
		exit(1);
	  }		
	}
}

/*****************************************************************************************************************************************************
	Method : GetStandardUsageOptionScreen
	
	Returns the HELP option screen
*****************************************************************************************************************************************************/
string optionsManager::GetStandardUsageOptionScreen() {

return	"netcat_part [OPTIONS] dest_ip [file] \n \
	-h Print this help screen\n \
	-v Verbose output \n \
	-m \"MSG\"	Send the message specified on the command line \n \
			Warning: if you specify this option, you do not specify a file.\n \
	-p 		port	Set the port to connect on (dflt: 6767) \n \
	-n bytes 	Number of bytes to send, defaults whole file \n \
	-o offset 	Offset into file to start sending \n \
	-l 		Listen on port instead of connecting and write output to file \n \
			and dest_ip refers to which ip to bind to (dflt: localhost)";		

}
	
/*****************************************************************************************************************************************************
	Method : getOptionDictionary

	Returns the option dictionary 		
*****************************************************************************************************************************************************/
map<string,string> optionsManager::getOptionDictionary() {

	return optionDict;
}
