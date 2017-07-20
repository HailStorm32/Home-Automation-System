//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//					Credit to Tom Archer & Rick Leinecker for the Serial class					//
//								Source site: https://goo.gl/KftQEJ								//
//																							    //
//**********************************************************************************************//
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "serial.h" 
#include "SysTime.h"

using namespace std;

const int startupWaitTime = 13000; //How long it takes the hub to startup
const int MESSAGE_SIZE = 17;
const int NUM_OF_FILES = 3;
const int MAX_NUM_OF_RETRIES = 4;
const string FILE_NAME[NUM_OF_FILES] = { "Log_9001.csv", "Log_9002.csv", "Log_9003.csv" }; //only have three right now, but will have a total of 8 later

class Hub
{
public:

	Hub(int comPort);

	~Hub();

	/*
	Description:
		Make a COM conection to the first hub
	Return:
		True -- made connection
		False -- no connection
	*/
	bool begin(bool withPings = true);

	/*
	Input:
		hubAddress(I) -- Address of the hub you want sensor data from
					(9001, 9002, 9003, 9004, 9005, 9006, 9007 or 9008)
	Returns:
		string -- "coded" string containing sensor data
	*/
	string requestData(int hubAddress);

	/*
	Input:
		codedData(I) -- String containing "coded" sensor data
	Description:
		Prints sensor data to the console
	*/
	void printData(string codedData);

	/*
	Input:
		codedData(I) -- String containing "coded" sensor data
	Description:
		Puts the data into a .csv file
	*/
	void storeData(string codedData);

	/*
	Description:
		Restarts the first hub by closing and reopening COM connection
	Returns:
		True -- restarted successfully and was able to reconnect via COM
		False -- restart failed and was NOT able to reconnect via COM
	*/
	bool restart();

private:
	CSerial serial;
	fstream file;
	SysTime clock;

	int comPort;
	int numOfRetries;

	/*
	Input:
		codedMessage (I) -- string of coded message
		temperature (O)
		motion (O)
		fromAddress (O)
	Returns:
		True -- decoded successfully
		False -- NOT decoded successfully
	*/
	bool decodeData(float &temperature, int &motion, int &fromAddress, int &toAddress, const string &codedData);

	/*
	Input:
		codedMessage (I) -- string of coded message
	Returns:
		True -- message was formated correctly
		False -- message was NOT formated correctly
	*/
	bool isValidData(const string& codedData);
};