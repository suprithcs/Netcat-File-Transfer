#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <map>
#include <regex>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include "optionsManager.h"
#include "fileOperationsManager.h"



optionsManager opManager;
fileOperationsManager fileManager;
netcatLogger logger;



using namespace std;


/**************************************************************************************************************************************************


	Method : networkConnectClient
	Arguments :
		socketFileDesc : Socket File Descriptor,
		serverNameOrIP : IPADDRESS OR HOSTNAME, 
		portNumber : PORT NUMBER, 
		serverAddressInfo : struct addrinfo to get all the required members filled from getAddrinfo, 
		res : pointer to the linkedlist of the addrinfo structs 
	

	Connects the client to the IPAddress or Hostname
******************************************************************************************************************************************************/

int networkConnectClient( int socketFileDesc,const char* serverNameOrIP, const char* portNumber, struct addrinfo* serverAddressInfo, struct addrinfo *res) {

	int conStatus;		



	if((conStatus = getaddrinfo(serverNameOrIP, portNumber, serverAddressInfo, &res))!= 0) {			
		cout <<" FAILED TO GET DESTINATION INFORMATION  : " << gai_strerror(errno) << "\n" ; 
	} 
	else {
		if((conStatus = connect(socketFileDesc,res->ai_addr,res->ai_addrlen)) != 0) {
			cout << " CONNECTION ERROR : " <<  strerror(errno) << "\n";
		}
	}
	return conStatus;
}


/**************************************************************************************************************************************************

	Method : getDigest

	Arguments :
		text : pointer to a message

	Returns the message digest of the text
**************************************************************************************************************************************************/
string getDigest(const char* text) {

	cout << "PREPARING DIGEST... " << endl;
	unsigned char* hashtext;	
	/*string keyString { to_string(0xe2) }; // THE KEY IS HARDCODED

	const char *key = keyString.c_str();	
	cout<< " KEY : " << key << endl ;		*/
	static const int key[16] = { 0xfa, 0xe2, 0x01, 0xd3, 0xba, 0xa9,
0x9b, 0x28, 0x72, 0x61, 0x5c, 0xcc, 0x3f, 0x28, 0x17, 0x0e };
	hashtext = HMAC(EVP_sha1(), key, sizeof key/sizeof key[0], (unsigned char*)text, strlen(text), NULL, NULL);
	char digestString[40];
	for(int i=0; i < 20 ; i++) {		
		sprintf(digestString+i*2, "%02x", (unsigned int) hashtext[i]);
	}	
	return string(digestString);	
}

/**************************************************************************************************************************************************
	Method : digestCompare

	Arguments : 
		clientDigest : Digest recieved from the client
		serverDigest : Digest recieved from the	serverDigest

	Compares the two digests and returns 0 on success
**************************************************************************************************************************************************/
int digestCompare(string clientDigest, string serverDigest) {
	return clientDigest.compare(serverDigest);
}


/**************************************************************************************************************************************************
	Method : startNetcatPartServer

	Arguments :
		fileName : File name where the content needs to be written
		ipAddress : IP of the server
		portNumber : port number on which the application starts

	Starts and manages the server. Recieves the data from client, matches the digest and writes the data to a file
			
**************************************************************************************************************************************************/
int startNetcatPartServer(char* fileName,const char* ipAddress,const char* portNumber) {

	cout << "PORT NO: " << portNumber <<endl;
	struct addrinfo netCatPartServerInfo , *ptrToListOfServerInfo;
	struct sockaddr clientAddressInfo;
	char *sockReadBuffer;
	vector<char> sockWriteBuffer;
	socklen_t sizeOfClientAddrStr;
	string msgDigest;
	static const int sockWriteBufferSize = 2048;
	int serverOpsStatus, serverSocketDescriptor, newClientSocket, recievedSize=0,clientCount =1;
	memset(&netCatPartServerInfo, 0, sizeof(netCatPartServerInfo));
	netCatPartServerInfo.ai_flags = AI_PASSIVE;
	
	logger.logMeOnVerbose("ENTERING SERVER CODE IMPLEMENTATION");

	if((serverOpsStatus = getaddrinfo(NULL, portNumber, &netCatPartServerInfo, &ptrToListOfServerInfo))!= 0) 
	{			
		cout <<" FAILED TO GET HOST MACHINE'S INFORMATION NEEDED FOR SERVER CREATION  : " << gai_strerror(errno) << "\n" ; 

	} 
	else 
	{
		serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
		if(serverSocketDescriptor == -1) { //error
	 		cout <<" ERROR IN OPENING SOCKET FOR SERVER OPERATIONS " << strerror(errno);

		}
		else 
		{
			logger.logMeOnVerbose("SOCKET HAS BEEN CREATED");	

			if((serverOpsStatus = bind(serverSocketDescriptor,ptrToListOfServerInfo->ai_addr,ptrToListOfServerInfo->ai_addrlen)) != 0) {
				cout << " SERVER SOCKET BINDING ERROR : " <<  strerror(errno) << "\n";
			}
			else
			{
				logger.logMeOnVerbose("SOCKED BINDING HAS BEEN DONE");				

				if((serverOpsStatus = listen(serverSocketDescriptor, 10))!=0)
				{
					cout << "SERVER IS UNABLE TO LISTEN ON THE SELECTED PORT" << strerror(errno) << endl;
				}
				else
				{
					logger.logMeOnVerbose("SERVER IS LISTENING ON THE CHOSEN PORT AND CAN ACCEPT CONNECTION REQUESTS FROM 10 CLIENTS AT A TIME");
					cout << "SERVER IS READY TO ACCEPT CONNECTIONS"<< endl;
					while(1)
						{
							sizeOfClientAddrStr = sizeof(clientAddressInfo);				 						
							sockReadBuffer = (char *)malloc(2048);	
							sockWriteBuffer.clear();					
							
							if((newClientSocket = accept(serverSocketDescriptor, &clientAddressInfo, &sizeOfClientAddrStr)) == -1)
							{
								cout << "CLIENT CONNECTION FAILED" << strerror(errno) << endl;
							}
							else
							{
								logger.logMeOnVerbose("SERVING CLIENT NUMBER: " + to_string(clientCount++)); 
								sockWriteBuffer.erase(sockWriteBuffer.begin(),sockWriteBuffer.end());
								memset(sockReadBuffer,0,2048);
								recievedSize = recv(newClientSocket, sockReadBuffer, 2048, 0);
								while (recievedSize > 0)						
								 {								
						
									sockWriteBuffer.insert(sockWriteBuffer.end(), sockReadBuffer, sockReadBuffer + recievedSize);									
									logger.logMeOnVerbose("RECIEVED NUMBER OF BYTES: " + to_string(recievedSize));
									recievedSize = recv(newClientSocket, sockReadBuffer, 2048, 0);
				
								}
								
								if(sockWriteBuffer.size() > 0) {
									cout << " COMPLETE DATA HAS BEEN RECIEVED: " << endl;
									string stringedData = string (sockWriteBuffer.begin(), sockWriteBuffer.end());
									cout << "stringedData:" << stringedData << endl;
									stringedData += "\0"; 
									size_t pos = stringedData.find("|");
									string data = stringedData.substr(0, pos);
									string hashedData = stringedData.substr(pos+1,stringedData.length()-1);
									
									logger.logMeOnVerbose("CLIENT DIGEST: " +hashedData);	
									msgDigest = getDigest(data.c_str());
									logger.logMeOnVerbose("SERVER DIGEST : " +msgDigest);
									logger.logMeOnVerbose("COMPARING THE CLIENT AND SERVER DIGESTS");
								 	if(digestCompare(hashedData, msgDigest) == 0) {
										cout << "MESSAGE INTEGRITY VERIFIED AND MATCHED" <<endl;
										if(data.length() > 0) 
											fileManager.writeToFile(data.c_str(), fileName);		
										else
											logger.logMeOnVerbose("NOT WRITING TO FILE AS EMPTY STRING HAS BEEN  RECIEVED");
									}
								  	else
										cout << "MESSAGE INTEGRITY VERIFIED AND DATA DOES NOT MATCH " <<endl;
									
								}
								else
									cout << " NO DATA RECIEVED " << endl;
								free(sockReadBuffer);
							}
							close(newClientSocket);	
						}
					
				}			
			}
		}	
	}
}

/**************************************************************************************************************************************************
	Method : clientConnect
	Arguments :	
		serverNameOrIP - IP Address
		fileName - client reads from this file
		portNo - port Number to which the client must connect

	All the client communication activities like opening socket, forming a connection, reading and sending the data.
**************************************************************************************************************************************************/
int clientConnect(const char* serverNameOrIP,char* fileName,const char* portNo) {
	
	int socketFileDesc;
	struct addrinfo serverAddressInfo, *res;
	//Set the address Info
	memset(&serverAddressInfo, 0, sizeof serverAddressInfo);
	serverAddressInfo.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	serverAddressInfo.ai_socktype = SOCK_STREAM;
	int clientStatus = 1;	
	socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
	if(socketFileDesc == -1) { //error
 		cout <<" ERROR IN OPENING SOCKET " << strerror(errno);
	}
	else {
		map<string,string> opDict = opManager.getOptionDictionary();	
		int conStatus = networkConnectClient(socketFileDesc,serverNameOrIP,portNo,&serverAddressInfo, res);	
		cout << "STATUS " << conStatus;	
		if(conStatus == 0) { //CONNECTION SUCCESS 
			const char *msg;
			auto value = opDict.find("message");	
			if(value != opDict.end()) {
				msg = (value->second).c_str();
			}
			else {		
				//READING FROM FILE		
				int noOfBytes = 0, offset = 0;
				value = opDict.find("offset");				
				if(value != opDict.end()) 
					 offset = atoi((value->second).c_str());

				value = opDict.find("nBytes");
				if(value != opDict.end()) 
					noOfBytes = atoi((value->second).c_str());
				logger.logMeOnVerbose("OFFSET : " +to_string(offset));
				logger.logMeOnVerbose("NUMBER OF BYTES: " +to_string(noOfBytes));
				msg = fileManager.readFromFile(fileName,offset,noOfBytes,logger);				
			}

			string hashedData = getDigest(msg);	
			hashedData =  string(msg) + "|" + hashedData;
			logger.logMeOnVerbose("SENT DATA : " + hashedData);	
			int bytes_sent = send(socketFileDesc,hashedData.c_str(), hashedData.length(), 0);		
			clientStatus = 0;
		}
	}
	return clientStatus;		
}

/**************************************************************************************************************************************************

	Method : processCommand

	serverNameOrIP - IP Address
	fileName : FileName either to read data from file in the client OR  write to the file in case of server.

	Processes the commands from the option dictionary and takes appropriate actions
**************************************************************************************************************************************************/

int processCommand(const char* serverNameOrIP,char* fileName) {

	logger.logMeOnVerbose(" IN PROCESS COMMAND");
	int returnVal = 0;	
	map<string,string> opDict = opManager.getOptionDictionary();	
	auto value = opDict.find("verbose");				
	if(value != opDict.end()) 
		logger.verboseState = 1;
	
	string portNo = "7777";	//Default Port for Client Connection
	value = opDict.find("port");				
	if(value != opDict.end()) 
		portNo = (value->second);
	
	value = opDict.find("server");				
	if(value != opDict.end())  {
		cout << "STARTING THE SERVER..." << endl;		
		returnVal = startNetcatPartServer(fileName,serverNameOrIP,portNo.c_str());		
	}
	else 
		returnVal = clientConnect(serverNameOrIP,fileName,portNo.c_str());	
	return returnVal;
}




/**************************************************************************************************************************************************
	Method : getFileName
	Arguments:
		argc : No of arguments from the command line
		argv : All command line arguments are stored as an array
		
	Returns the File name mentioned in the command line arguments
**************************************************************************************************************************************************/
bool isFileNameGiven(char* fileName) {

	bool status = false;
	
	if(strstr(fileName,".txt") || strstr(fileName,".eng")) {					
		cout << "FILE NAME : " << fileName;
		status = true;
	}		
	return status;	
} 


/**************************************************************************************************************************************************
	Method : main
	Arguments:
		argc : No of arguments from the command line
		argv : All command line arguments are stored as an array
		
	THe main function displays the help screen , displays the program mode (Client or Server) and sends it for further processing.
**************************************************************************************************************************************************/
int main(int argc, char *argv[]) {
		
	int status;	
	if (argc < 2) {
		fprintf(stderr,"for usage: netcat_part -h\n");
		return 1;
	}
	opManager.readOptions(argc,argv);
	map<string,string> opDict = opManager.getOptionDictionary();		
	bool isClient = true;
	auto val = opDict.find("server");
	if(val != opDict.end()) 
		isClient = false;
	val = opDict.find("help");
	if(val == opDict.end()) {
		cout << " CLIENT MODE : " << isClient << endl;
		cout << " SERVER MODE : " << !isClient << endl;
	

		char *fileName = NULL,*ipAddress = NULL;
		bool isFile = isFileNameGiven(argv[argc-1]);
							
		if(isFile) {	
			fileName = argv[argc-1];
			ipAddress = argv[argc-2];
		}
		else
			ipAddress = argv[argc-1];			

		if(!isClient) {
			string checkIP = string(ipAddress);
			if(all_of(checkIP.begin(),checkIP.end(),::isdigit) || checkIP == "-l" || checkIP == "-v")
				ipAddress = NULL;
		}		
		if(ipAddress == NULL && isClient) {
			cout << "\n NO IP ADDRESS MENTIONED. PLEASE USE THE HELP(-h) OPTION TO CHECK THE USAGE " << endl;
		}
		else {	
			if(processCommand(ipAddress,fileName) == 0) 
				cout << endl << " PROGRAM EXECUTED SUCCESSFULLY " << endl;	
			else
				cout << endl << " PROGRAM HAS ENCOUNTERED ERRORS " << endl;
		}
	}
	else
		cout << val->second << endl<<endl;
	return 0;
}


