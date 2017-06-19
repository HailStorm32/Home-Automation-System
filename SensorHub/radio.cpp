//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are licensed by their respective owners		//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//
#include <SPI.h>
#include "nRF24L01.h"
#include <RF24.h>
#include "radio.h"



Radio::Radio(const int myAddress, const int range[], RF24* radioPointer, Hub* systemPointer)
{
	//Setup the pointer to the RF24 radio class
	radioP = radioPointer;

	//Setup the pointer to the Hub system class
	//This will gives us access to errorReport
	systemP = systemPointer;

	//Set variables
	this->myAddress = myAddress;
	fromAddress = myAddress;

	for (int indx = 0; indx < RANGE_SIZE; indx++)
	{
		this->range[indx] = range[indx];
	}
}

bool Radio::sendData(float temperature, int motion, int fromAddress, int toAddress)
{
	String codedMessage;

	//Pack the data into a single string
	codedMessage = encodeMessage(temperature, motion, fromAddress);

	digitalWrite(DEBUG_LED, HIGH);

	radioP->openWritingPipe(toAddress);
	radioP->stopListening();

	if (radioP->write(codedMessage.c_str(), MESSAGE_SIZE))
	{
		Serial.println("true");
		digitalWrite(DEBUG_LED, LOW);
		return true;
	}
	else
	{
		///Serial.println("Error Sending...");
		systemP->errorReport(3, 9001);
		digitalWrite(DEBUG_LED, LOW);
		return false;
	}
}

bool Radio::receiveData()
{
	int address = 0;
	char codedMessageCStr[MESSAGE_SIZE] = {};
	String codedMessage;
	float temp = 0;
	int motion = 0;
	int fromAddress = 0;

	radioP->openReadingPipe(0, myAddress); //must be set to receiver's address. Will only receive data from hubs that have opened writing pipes to this address
	radioP->startListening();

	waitForData(-1);
	Serial.println(sizeof(codedMessageCStr));
	radioP->read(codedMessageCStr, MESSAGE_SIZE);

	codedMessage = codedMessageCStr;

	decodeMessage(temp, motion, fromAddress, codedMessage);

	Serial.println(temp);
	Serial.println(motion);
	Serial.println(fromAddress);

	return true;
}

bool Radio::startupPings()
{
	bool pingsSuccess = false;
	int debug = 0; //for debug only

	if (myAddress == 9001)
	{

		for (int i = 0; i < 5; i++)
		{

			debug = range[i];//debug only

			if (sendPing(i) == true && receiveAcknowledge(i) == true)
			{
				Serial.println("Successful contact with hub ");//debug only
				Serial.println(debug);//debug only
			}
		}

	}
	else
	{
		Serial.println("Waiting for pings...");//debug only
		pingsSuccess = receivePing();
	}

	return pingsSuccess;
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

bool Radio::decodeMessage(float &temperature, int &motion, int &fromAddress, const String &codedMessage)
{
	String stringHolder;
	int indx = 0;
	int secondIndx = 0;
	char messageData[5];

	strcpy(message, codedMessage.c_str());

	if (isValidMessage(codedMessage) == false)
	{
		systemP->errorReport(11);
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
	int targetTime = 0;

	if (waitTime >= 0)
	{
		targetTime = millis() + waitTime;
		while (!radioP->available() && millis() < targetTime) {}

		if (millis() > targetTime)
		{
			//Serial.println("ERROR! timeout!");//debug only
			systemP->errorReport(10);
			return false;
		}
	}
	else
	{
		while (!radioP->available()) {}
	}

	return true;
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

bool Radio::receivePing()
{
	bool pingReceive = false;
	int targetTime = 0;
	int toAddress = 0;
	int fromAddress = 0;

	radioP->openReadingPipe(1, myAddress);
	radioP->startListening();


	do
	{
		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Receive the "toAddress" //////////////////////////
		targetTime = millis() + 30000;
		while (!radioP->available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 30sec

		if (radioP->available())
		{
			Serial.println("Receiving Data..."); //debug only
			radioP->read(&toAddress, sizeof(toAddress));
		}
		else if (millis() > targetTime)
		{
			//Serial.println("ERROR!!!");
			systemP->errorReport(9);
		}

		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Receive the "fromAddress" //////////////////////////
		targetTime = millis() + 30000;
		while (!radioP->available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 30sec

		if (radioP->available())
		{
			radioP->read(&fromAddress, sizeof(fromAddress));
		}
		else if (millis() > targetTime)
		{
			//Serial.println("ERROR!!!");
			systemP->errorReport(9);
		}

		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Make Sure we received the right address ///////////////////
		if (toAddress == myAddress)
		{
			radioP->stopListening();
			radioP->openWritingPipe(range[0]);

			Serial.println("Sending Ping back..."); //debug only

			if (radioP->write(&myAddress, sizeof(myAddress)))
			{
				pingReceive = true;
			}
			else
			{
				Serial.println("ERROR!!!");
				systemP->errorReport(3, range[0]);
			}
		}

	} while (toAddress != myAddress); //Loop until we receive the address that we want

	return pingReceive;
}

bool Radio::sendPing(int index)
{
	int toAddress = 0;
	int targetTime = 0;

	Serial.println(" ");//debug only
	Serial.println("Sending pings...");//debug only

	radioP->stopListening();
	radioP->openWritingPipe(range[index]);

	//\\\\\\\\\\\\\\\\\\\\\\\\\ Send "To Address" //////////////////////////

	toAddress = range[index];

	if (!radioP->write(&toAddress, sizeof(toAddress)))
	{
		//Serial.println("ERROR!!!");
		systemP->errorReport(3, toAddress);
		return false;
	}

	delay(200);

	//\\\\\\\\\\\\\\\\\\\\\\\\ Send "from address" ////////////////////////
	if (!radioP->write(&myAddress, sizeof(myAddress)))
	{
		//Serial.println("ERROR!!!");
		systemP->errorReport(3, toAddress);
		return false;
	}


	return true;
}

bool Radio::receiveAcknowledge(int index)
{
	bool didAcknowledge = false;
	int targetTime = 0;
	int data = 0;
	int acknowledgerAddress = range[index];

	radioP->openReadingPipe(1, myAddress);
	radioP->startListening();

	targetTime = millis() + 1000;

	while (!radioP->available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 1sec

	if (radioP->available())
	{
		radioP->read(&data, sizeof(data));

		if (data == acknowledgerAddress)
		{
			Serial.println("Received acknowledge...");//debug only
			didAcknowledge = true;
		}
		else
		{
			//Serial.println("ERROR!!! Incorrect acknowledgement ping recieved");
			systemP->errorReport(8, acknowledgerAddress); //Report that we got the wrong data back
			didAcknowledge = false;
		}
	}
	else if (millis() > targetTime)///we went over time, report it
	{
		//Serial.println("ERROR!!! No acknowledgement ping recieved");
		systemP->errorReport(7, acknowledgerAddress);
		didAcknowledge = false;
	}

	return didAcknowledge;
}