#include <SPI.h>
#include "nRF24L01.h"
#include <RF24.h>
#include "radio.h"

//RF24 radio(9, 10);

Radio::Radio(unsigned int myAddress, unsigned int errorLed)
{
	//temperature = 0;
	//motion = 0;
	fromAddress = myAddress;
	//toAddress;
	this->errorLed = errorLed;
}

bool Radio::sendData(float temperature, int motion, int fromAddress, int toAddress)
{
	return false;
}

bool Radio::receiveData()
{
	return false;
}


String Radio::encodeMessage(float temperature, int motion, int fromAddress)
{
	/*
	Follows the following format:

	fromAddress -> spacer -> temperature -> spacer -> motion -> end char

	Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
	Array index slot -->[9,0,0,1,-,7,4,.,5,6,-, 5, 8, 9, x]

	9001-74.36-589x
	*/

	if ((fromAddress < 9001 || fromAddress > 9999) || (motion < 0 || motion > 9999) || (temperature < 0 || temperature > 9999))
	{
		return "ERROR";
	}

	String numberHolder;
	String messageToReturn;

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

	//Enter the end char
	message[numberHolder.length() + 11] = 'x';

	messageToReturn = message;

	//Serial.println(message);//debug only

	return messageToReturn;
}


bool Radio::decodeMessage(float &temperature, int &motion, int &fromAddress,const String &codedMessage)
{
	String stringHolder;
	int indx = 0;
	int secondIndx = 0;
	char messageData[5];

	strcpy(message, codedMessage.c_str());

	if (isValidMessage(codedMessage) == false)
	{
		//Error report #011
		Serial.println(" ");
		Serial.println("ERROR 011!! Mesage not formated!!");

		digitalWrite(errorLed, HIGH);
		delay(2000);
		digitalWrite(errorLed, LOW);

		return false;
	}


	//Get fromAddress data
	while (message[indx] != '-')
	{
		//Serial.println(message[indx]);
		stringHolder += message[indx];
		indx++;
	}   

	fromAddress = atoi(stringHolder.c_str()); //Convert string into int
	//Serial.println(fromAddress);//debug only
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

bool Radio::isValidMessage(const String &codedMessage)
{
	unsigned int spacerCount = 0;

	//count the spacers ( - ) and end char ( x )
	for (int indx = 0; indx < codedMessage.length(); indx++)
	{
		if (codedMessage[indx] == '-' || codedMessage[indx] == 'x')
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
