#include <SPI.h>
#include "nRF24L01.h"
#include <RF24.h>
#include "radio.h"

//RF24 radio(9, 10);

Radio::Radio(int myAddress)
{
	temperature = 0;
	motion = 0;
	fromAddress = myAddress;
	toAddress;
}

bool Radio::sendData(float temperature, int motion, int fromAddress, int toAddress)
{
	return false;
}

bool Radio::receiveData()
{
	return false;
}


void Radio::encodeMessage(float temperature, int motion, int fromAddress)//, char codedMessage[])
{
	/*
	Follows the following format:

	fromAddress -> spacer -> temperature -> spacer -> motion -> end char

	Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
	Array index slot -->[9,0,0,1,-,7,4,.,5,6,-, 5, 8, 9, x]

	9001-74.36-589x
	*/

	String numberHolder;

	numberHolder = static_cast<String>(fromAddress);

	//Add the fromAddress into the cstring
	for (int i = 0; i < 4; i++)
	{
		message[i] = numberHolder[i];
	}

	message[4] = '-';


	//Add temperature data into the cstring
	numberHolder = static_cast<String>(temperature);

	for (int i = 0; i < 5; i++)
	{
		message[i + 5] = numberHolder[i];
	}

	message[10] = '-';

	//Add motion data into the cstring
	numberHolder = static_cast<String>(motion);

	for (int i = 0; i < numberHolder.length(); i++)
	{
		message[i + 11] = numberHolder[i];
	}

	message[numberHolder.length() + 11] = 'x';

	Serial.println(message);//debug only
}

void Radio::decodeMessage(float &temperature, int &motion, int &fromAddress)//char codedMessage[], float &temperature, int &motion, int &fromAddress)
{
	String stringHolder;
	int indx = 0;
	int secondIndx = 0;

	/*
	TO-DO:
		Add checker here that will only proceed if 'motion' is NOT null
	*/


	//Get fromAddress data
	while (message[indx] != '-')
	{
		stringHolder[indx] = message[indx];
		indx++;
	}

	fromAddress = atoi(stringHolder.c_str()); //Convert string into int

	indx++;

	//Get temperature data
	while (message[indx] != '-')
	{
		stringHolder[secondIndx] = message[indx];
		indx++;
		secondIndx++;
	}

	temperature = atof(stringHolder.c_str()); //Convert string into int

	indx++;
	secondIndx = 0;

	//Get motion data
	while (message[indx] != 'x')
	{
		stringHolder[secondIndx] = message[indx];
		indx++;
		secondIndx++;
	}

	motion = atoi(stringHolder.c_str()); //Convert string into int

	//return true;
}

bool Radio::waitForData(int waitTime)
{
	/*int targetTime = 0;

	if (waitTime >= 0)
	{
		targetTime = millis() + waitTime;
		while (!radio.available() && millis() < targetTime) {}

		if (millis() > targetTime)
		{
			//errorReport(10);
		}
	}
	else
	{
		while (!radio.available()) {}
	}*/
}
