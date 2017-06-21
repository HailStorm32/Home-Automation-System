#include "Hub.h"

Hub::Hub(int comPort)
{
	this->comPort = comPort;

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
}


void Hub::begin()
{
	if (serial.Open(comPort, 9600))
	{
		cout << "Opened!" << endl;

		cout << "Waiting for hub to startup..." << endl;
		Sleep(startupWaitTime);
		cout << "Done!\n\n\n\n";
	}
	else
	{
		cout << "Failed!" << endl;
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

	cout << "\n\nWaiting for response..." << endl;
	while (serial.ReadDataWaiting() <= 0){}

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

	file << "02:35, " << temperature << ", " << motion << ",\nZ";

	file.close();
}

bool Hub::restart()
{
	return false;
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

bool Hub::decodeData(float & temperature, int & motion, int & fromAddress, const string & codedData)
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
	while (message[indx] != 'x')
	{
		stringHolder += message[indx];
		indx++;
		secondIndx++;
	}

	motion = atoi(stringHolder.c_str()); //Convert string into int

	return true;
}
