#include <stdio.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include "fileOperationsManager.h"


using namespace std;



/******************************************************************************************************************************************************
	METHOD: WRITETOFILE.
	ARGUMENTS: POINTER TO SERVER'S BUFFER AND TARGET FILENAME.
	FUNCTION: WRITES THE SERVER BUFFER'S CURRENT CONTENT TO THE GIVEN TARGET FILE.
	RETURNS 0 IF SUCCESS.
*******************************************************************************************************************************************************/	
int fileOperationsManager::writeToFile(const char* writeBuffer, const char *netcatPartDestFile){
	if(netcatPartDestFile == NULL) 
		netcatPartDestFile = "default.txt"; //DEFAULT FILE WRITTEN TO IF TEXT FILE IS NOT MENTIONED	
  	FILE * ptrToWrite = NULL;
	ptrToWrite = fopen (netcatPartDestFile,"w"); 
	if (ptrToWrite==NULL) //CONDITION TO CHECK FILE OPEN FAILURE
	  {
		cout << "\n ERROR IN OPENING FILE\n"; 
		return -1;
	  }
	else //WRITE THE CONTENT TO THE OPENED FILE
	  {
    		int bytesWritten;
    		bytesWritten = fwrite(writeBuffer,1,strlen(writeBuffer),ptrToWrite); //WRITE OPERATION
    		if(bytesWritten != strlen(writeBuffer))	
    		  {
			cout << "\n WRITE FAILED!!!";
			int fclose(FILE *ptrToWrite);
			ptrToWrite = NULL;			
			return -1;			
    		  }
		cout << "\nWRITE SUCCESS\n";
				
		fflush(ptrToWrite);
		fclose(ptrToWrite);
		ptrToWrite = NULL;
	  }
	return 0;
	
}
		
/*******************************************************************************************************************************************************
	METHOD: READFROMFILE
	ARGUMENTS: POINTER TO THE FILE THAT HAS TO BE READ, OFFSET AND NUMBER OF BYTES TO BE READ
	FUNCTION: READS A GIVEN FILE BASED ON THE GIVEN OFFSET AND NUMBER OF BYTES. DEFAULTS TO READ ENTIRE FILE IF BOTH OFFSET AND NUMBER OF BYTES IS NOT MENTIONED.
	RETURNS A POINTER TO BUFFER CONTAINING THE READ CONTENT.
*******************************************************************************************************************************************************/	
	
char* fileOperationsManager::readFromFile(const char *netcatPartInputFile, int seekToOffset, int readThisMuch,netcatLogger fileLogger){
	
	FILE * ptrToRead = NULL;
	int bytesRead = 0; 
	int noOfBytesToRead = 0;
	int totalNumberOfBytes = 0;
	char* readBuffer;
	ptrToRead = fopen (netcatPartInputFile,"r");
		if (ptrToRead==NULL) //CONDITION TO CHECK FILE OPEN FOR READ FAILURE
		  {
			cout << "ERROR IN OPENING FILE FOR READ OPERATION!\n ERROR: " << strerror(errno) << endl;
			cout << "ERRNO is:" << errno << "\n"; 
			return NULL;
		  }
		else
		  {
			if((fseek (ptrToRead , 0 , SEEK_END))==0) // FIND THE TOTAL NO OF BYTES AVAILABLE IN THE FILE
					{
						fileLogger.logMeOnVerbose("SEEK TO END OF FILE SUCCEEDED\n");				
						totalNumberOfBytes = ftell (ptrToRead);
						if(readThisMuch > totalNumberOfBytes)
							readThisMuch = totalNumberOfBytes;
						if(seekToOffset > totalNumberOfBytes)
							seekToOffset = 0;
						fileLogger.logMeOnVerbose("TOTAL NO. OF BYTES IN THE FILE IS: " + to_string(totalNumberOfBytes));
						rewind (ptrToRead);
					}
			else{cout << "FAILED IN FINDING THE TOTAL NO OF BYTES IN THE FILE!"; }
			if (seekToOffset == 0) // IF OFFSET IS NOT SPECIFIED BY THE USER
			{				
				if(readThisMuch ==0){ // READ THE ENTIRE FILE IF NUMBER OF BYTES IS NOT SPECIFIED
					noOfBytesToRead = totalNumberOfBytes;
					fileLogger.logMeOnVerbose("READING THE ENTIRE FILE AS NO REQUEST FOR SPECIFIC AMOUNT OF BYTES WAS MADE BY THE USER, WHICH IS: " + to_string(noOfBytesToRead));	
				}
				else
					noOfBytesToRead = readThisMuch;
			}
			else // IF OFFSET IS SPECIFIED BY THE USER
			{
				
				if((fseek (ptrToRead , seekToOffset, SEEK_SET))==0) //READ THE REST OF THE FILE IF USER REQUEST IS EITHER ZERO OR EXCEEDS THE NUMBER OF AVAILABLE BYTES FROM CURRENT OFFSET.
				{
					fileLogger.logMeOnVerbose("SUCCEEDED IN SEEKING TO GIVEN POSITION\n");
															
					if(readThisMuch == 0 || readThisMuch > (totalNumberOfBytes-ftell(ptrToRead))){
						fileLogger.logMeOnVerbose("READING REST OF THE FILE CONTENTS, AS THE NUMBER OF BYTES TO BE READ REQUESTED BY USER IS EITHER ZERO OR EXCEEDS THE AVAILABLE NUMBER OF BYTES\n");
					 	noOfBytesToRead = totalNumberOfBytes-ftell(ptrToRead);
					}						
					else{
						noOfBytesToRead = readThisMuch;
					}
				}
				else{cout << "FSEEK FAILURE!"; }
			}
			readBuffer = (char*) malloc (noOfBytesToRead+1); //ALLOCATE MEMORY FOR THE READ BUFFER BASED ON THE NUMBER OF BYTES TO BE READ.
			memset(readBuffer,0,sizeof(readBuffer));
  			if (readBuffer != NULL) {
				fileLogger.logMeOnVerbose("CURRENT POSITION OF THE FILE STREAM POINTER: "+ to_string(ftell(ptrToRead)));
				if((bytesRead = fread(readBuffer, 1, noOfBytesToRead, ptrToRead)) == noOfBytesToRead){ // READ OPERATION
					readBuffer[bytesRead] = '\0';							
					cout << "\nREAD SUCCESS!. BYTES READ INTO MEMORY: " << bytesRead; 
					fclose(ptrToRead);
				}
				else{
					cout << "\nREAD FAILURE! COULDN'T READ THE RELEVANT CHUNK OF THE FILE/ENTIRE FILE";	
					free (readBuffer);					
					return NULL;
				}
			}
			else{
				cout << "\nCOULDN'T ALLOCATE MEMORY TO READ BUFFER";
				fclose(ptrToRead);
				return NULL;
		    	}
		  }
	
	return readBuffer;		
}

