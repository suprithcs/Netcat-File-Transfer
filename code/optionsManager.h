#include <iostream>
#include <string>
#include <unistd.h>
#include <map>

using namespace std;

class optionsManager {
	
	map<string,string> optionDict;
	
	public : 
		
		void readOptions(int argc,char* argv[]);
		string GetStandardUsageOptionScreen();
		map<string,string> getOptionDictionary();
};
