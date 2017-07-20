//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//					Credit to Tom Archer & Rick Leinecker for the Serial class					//
//								Source site: https://goo.gl/KftQEJ								//
//																							    //
//**********************************************************************************************//
#include "Hub.h"

Hub::Hub(int comPort)
{
	this->comPort = comPort;
	numOfRetries = 0;

	//Setup the files
	for (int indx = 0; indx < NUM_OF_FILES; indx++)
	{
		file.open(FILE_NAME[indx]);
		file << "Time, Tempature, Motion,\nZ";
		file.close();
	}
}

Hub::~Hub()
{
	serial.Close();
}

bool Hub::begin(bool withPings)
{
	if (serial.Open(comPort, 9600))
	{
		cout << "Opened!" << endl;

		if (withPings)
		{
			Sleep(2000);
			serial.SendData("true", 4);//Tell hub it should do the startup pings
			cout << "Waiting for hub to startup..." << endl;
			Sleep(startupWaitTime);
		}
		else
		{
			Sleep(2000);
			serial.SendData("false", 5);//Tell hub it should do the startup pings
			cout << "Waiting for hub to startup..." << endl;
			Sleep(2000);
		}

		cout << "Done!\n\n\n\n";
		return true;
	}
	else
	{
		cout << "Failed!" << endl;
		return false;
	}
}

string Hub::requestData(int hubAddress)
{
	string requestToSend;
	char codedData[MESSAGE_SIZE];

	//Make sure the given address is in range
	if (hubAddress != 9001 && hubAddress != 9002 && hubAddress != 9003 && hubAddress != 9004 && hubAddress != 9005 && hubAddress != 9006 && hubAddress != 9007 && hubAddress != 9008)
	{
		cout << "ERROR!! Given address: (" << hubAddress << ") is out of bounds!" << endl;
		return "ERROR";
	}
	
	requestToSend = to_string(hubAddress);

	//Send request if we have a connection
	if (serial.IsOpened())
	{
		int nBytesSent = serial.SendData(requestToSend.c_str(), strlen(requestToSend.c_str()));
	}
	else
	{
		cout << "ERROR!! No connection to hub!" << endl;
		return "ERROR";
	}

	cout << "\n\nWaiting for response[" << requestToSend << "]..." << endl;
	//cout << "\n\nWaiting for response..." << endl;

	clock.startTimer(4);

	while (serial.ReadDataWaiting() <= 0 && !clock.timerPassed()){}

	//If it took to long to get a response
	if (clock.timerPassed())
	{
		clock.stopTimer();
		cout << "Took to long..." << endl;
		cout << "restarting..." << endl;

		restart();

		//Make we haven't retried to many times, and then try again
		if (numOfRetries < MAX_NUM_OF_RETRIES)
		{
			cout << "Requesting again..." << endl;
			numOfRetries++;
			return requestData(hubAddress);
		}
		else
		{
			numOfRetries = 0;
			return (requestToSend += "-0-0x");
		}
	}

	clock.stopTimer();
	numOfRetries = 0;

	serial.ReadData(codedData, MESSAGE_SIZE);

	return codedData;
}

void Hub::printData(string codedData)
{
	float temperature = 0;
	int motion = 0;
	int fromAddress = 0;

	//Only continue if the data can be decoded
	if (!decodeData(temperature, motion, fromAddress, codedData))
	{
		return;
	}

	cout << "\n\n\n---------------------" << endl;
	cout << "Temperature: " << temperature << "F" << endl;
	cout << "Motion: " << motion << " triggers" << endl;
	cout << "From: " << fromAddress << endl;
	cout << "\n\n\n";
}

void Hub::storeData(string codedData)
{
	float temperature = 0;
	int motion = 0;
	int fromAddress = 0;
	int address = 0;
	int pos = 0;
	char holder = ' ';

	//Only continue if the data can be decoded
	if (!decodeData(temperature, motion, fromAddress, codedData))
	{
		return;
	}

	address = (fromAddress - 9000) - 1;

	file.open(FILE_NAME[address]);

	//Find where the file ends
	while (!file.eof() && holder != 'Z')
	{
		file.get(holder);
	}
	//Get the position
	pos = file.tellp();
	file.seekp(pos-1);

	file << clock.getTime() << ", " << temperature << ", " << motion << ",\nZ";

	file.close();
}

bool Hub::restart()
{
	serial.Close();

	if (serial.Open(comPort, 9600))
	{
		Sleep(2000);
		serial.SendData("false", 5);
		Sleep(2000);
		return true;
	}
	else
	{
		return false;
	}
}

bool Hub::isValidData(const string & codedData)
{
	unsigned int spacerCount = 0;

	//count the spacers ( - ) and end char ( x )
	for (int indx = 0; indx < codedData.length(); indx++)
	{
		if (codedData[indx] == '-' || codedData[indx] == 'x')
		{
			spacerCount++;
		}
	}

	//If message was formated correctly, the variable should equal 3
	if (spacerCount == 3)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Hub::decodeData(float & temperature, int & motion, int & fromAddress, int &toAddress, const string & codedData)
{
	string stringHolder;
	char message[MESSAGE_SIZE];
	int indx = 0;
	int secondIndx = 0;
	char messageData[5];

	strcpy(message, codedData.c_str());

	if (isValidData(codedData) == false)
	{
		cout << "ERROR! Coded data not formated: (" << codedData << ")!!" << endl;
		return false;
	}

	indx = 0;

	//Get fromAddress data
	while (message[indx] != '-')
	{
		////Serial.println(message[indx]);
		stringHolder += message[indx];
		indx++;
	}

	fromAddress = atoi(stringHolder.c_str()); //Convert string into int

	indx++;

	stringHolder = "";

	//Get temperature data
	while (message[indx] != '-')
	{
		stringHolder += message[indx];
		indx++;
		secondIndx++;
	}

	temperature = atof(stringHolder.c_str()); //Convert string into int

	indx++;
	secondIndx = 0;

	stringHolder = "";

	//Get motion data
	while (message[indx] != '-')
	{
		stringHolder += message[indx];
		indx++;
		secondIndx++;
	}

	motion = atoi(stringHolder.c_str()); //Convert string into int

	indx++;
	stringHolder = "";

	//Get toAddress data
	while (message[indx] != 'x')
	{
		stringHolder += message[indx];
		indx++;
	}

	toAddress = atoi(stringHolder.c_str());

	return true;
}
