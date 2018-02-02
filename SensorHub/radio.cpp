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
	//fromAddress = myAddress;
	numOfRetries = 0;

	for (int indx = 0; indx < RANGE_SIZE; indx++)
	{
		this->range[indx] = range[indx];
	}
}

bool Radio::sendData(float temperature, int motion, int toAddress, int fromAddress, int forwardAddress)
{
	String codedMessage;
	static bool sentStandBy = false;
	bool requestSucceeded = true;

	if (forwardAddress == 0)
	{
		forwardAddress = toAddress;
	}

	//Will use "myAddress" as the from address if argument is left blank (it should be if we are NOT forwarding)
	if (fromAddress == 0)
	{
		systemP->debugPrint("Using myAddress");//Debug
		//Pack the data into a single string
		codedMessage = encodeMessage(temperature, motion, myAddress, toAddress);
	}
	else
	{
		systemP->debugPrint("Using default");//Debug
		//Pack the data into a single string
		codedMessage = encodeMessage(temperature, motion, fromAddress, toAddress);
	}

	digitalWrite(DEBUG_LED, HIGH);

	radioP->stopListening();
	radioP->openWritingPipe(forwardAddress);

	radioP->flush_tx();

	systemP->debugPrint("Sending to: ");//Debug only
	systemP->debugPrint(toAddress);//Debug only
	systemP->debugPrint("forwardAddress: ");//Debug only
	systemP->debugPrint(forwardAddress);//Debug only

	//If we are sending data to a "slave" hub, we need to let them know by sending a listening request
	if (forwardAddress != 9001 && sentStandBy == false)
	{
		requestSucceeded = sendRequest(forwardAddress, 'F', 0, 0, false);
		sentStandBy = true;
	}

	if (radioP->write(codedMessage.c_str(), MESSAGE_SIZE))
	{
		systemP->debugPrint("true");
		digitalWrite(DEBUG_LED, LOW);
		radioP->txStandBy();
		numOfRetries = 0;
		return true;
	}
	//If sending failed, retry up to 4 times
	else
	{
		if (numOfRetries < MAX_NUM_OF_RETRIES && requestSucceeded == true)
		{
			systemP->debugPrint("Trying again...");
			numOfRetries++;
			delay(500);
			sendData(temperature, motion, toAddress, fromAddress, forwardAddress);
		}
		//If sending still failed, try forwarding to other hubs
		else if (numOfRetries >= MAX_NUM_OF_RETRIES || requestSucceeded == false)
		{
			numOfRetries = 0;
			sentStandBy = false; //Reset the flag because we are going to be sending to another address


			if (toAddress < myAddress && (forwardAddress != myAddress))
			{
				//Start working our way from the forwardAddress and finding a hub we can forward the message to
				forwardAddress++;

				//If we have ranout of address to try
				if (forwardAddress > myAddress || forwardAddress == myAddress)
				{
					systemP->debugPrint("Failed: ranout of address");//Debug only
					return false;
				}

				if (!sendData(temperature, motion, toAddress, fromAddress, forwardAddress))
				{
					return false;
				}
				else
				{
					systemP->debugPrint("true");
					digitalWrite(DEBUG_LED, LOW);
					radioP->txStandBy();
					numOfRetries = 0;
					return true;
				}
			}
			else
			{
				//Start working our way from the forwardAddress and finding a hub we can forward the message to
				forwardAddress--;
				sentStandBy = false; //Reset the flag because we are going to be sending to another address

				//If we have ranout of address to try
				if (forwardAddress < myAddress || forwardAddress == myAddress)
				{
					systemP->debugPrint("Failed: ranout of address");//Debug only
					return false;
				}

				if (!sendData(temperature, motion, toAddress, fromAddress, forwardAddress))
				{
					return false;
				}
				else
				{
					systemP->debugPrint("true");
					digitalWrite(DEBUG_LED, LOW);
					radioP->txStandBy();
					numOfRetries = 0;
					return true;
				}
			}

			systemP->errorReport(3, toAddress);
			digitalWrite(DEBUG_LED, LOW);
			numOfRetries = 0;
			return false;
		}
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
	int toAddress = 0;

	radioP->openReadingPipe(0, myAddress); //must be set to receiver's address. Will only receive data from hubs that have opened writing pipes to this address
	radioP->startListening();

	waitForData(-1);
	systemP->debugPrint(sizeof(codedMessageCStr));
	radioP->read(codedMessageCStr, MESSAGE_SIZE);

	codedMessage = codedMessageCStr;

	decodeMessage(temp, motion, fromAddress, toAddress, codedMessage);

	//If the data was for us
	if (toAddress == myAddress)
	{
		radioP->flush_rx();

		radioP->closeReadingPipe(0);

		Serial.print(codedMessage);//Send to the command center

		systemP->debugPrint(temp); //Debug only
		systemP->debugPrint(motion);//Debug only
		systemP->debugPrint(fromAddress);//Debuf only
	}
	//If the data was NOT for us
	else
	{
		//Forward the data
		sendData(temp, motion, toAddress, fromAddress);
	}



	return true;
}

bool Radio::sendRequest(int toAddress, char command, int forwardAddress, int fromAddress, bool tryForwarding)
{
	String codedMessage;

	if (forwardAddress == 0)
	{
		forwardAddress = toAddress;
	}

	if (fromAddress == 0)
	{
		codedMessage = encodeMessage(toUpperCase(command), toAddress, myAddress);
	}
	else
	{
		codedMessage = encodeMessage(toUpperCase(command), toAddress, fromAddress);
	}


	radioP->stopListening();
	radioP->openWritingPipe(forwardAddress);

	digitalWrite(DEBUG_LED, HIGH);

	systemP->debugPrint("Sending to: ");//Debug only
	systemP->debugPrint(toAddress);//Debug only
	systemP->debugPrint("forwardAddress: ");//Debug only
	systemP->debugPrint(forwardAddress);//Debug only

	if (radioP->write(codedMessage.c_str(), MESSAGE_SIZE))
	{
		digitalWrite(DEBUG_LED, LOW);
		radioP->txStandBy();
		numOfRetries = 0;
		return true;
	}
	//If sending failed, retry up to 4 times
	else
	{
		if (numOfRetries < MAX_NUM_OF_RETRIES)
		{
			systemP->debugPrint("Trying request again...");
			numOfRetries++;
			delay(500);
			sendRequest(toAddress, toUpperCase(command), forwardAddress);
		}
		//If sending still failed, try forwarding to other hubs
		else if (numOfRetries >= MAX_NUM_OF_RETRIES && tryForwarding == true)
		{
			numOfRetries = 0;

			if (toAddress < myAddress && (forwardAddress != myAddress))
			{
				//Start working our way from the forwardAddress and finding a hub we can forward the message to
				forwardAddress++;

				systemP->debugPrint("\n In num1!!");//Debug only

				//If we have ranout of addresses to try
				if (forwardAddress > myAddress || forwardAddress == myAddress)
				{
					systemP->debugPrint("Failed: ranout of address");//Debug only
					return false;
				}

				if (!sendRequest(toAddress, toUpperCase(command), forwardAddress))
				{
					return false;
				}
				else
				{
					systemP->debugPrint("true");
					digitalWrite(DEBUG_LED, LOW);
					radioP->txStandBy();
					numOfRetries = 0;
					return true;
				}
			}
			else
			{
				//Start working our way from the forwardAddress and finding a hub we can forward the message to
				forwardAddress--;

				systemP->debugPrint("\n In num2!!");//Debug only

				//If we have ranout of addresses to try
				if (forwardAddress < myAddress || forwardAddress == myAddress)
				{
					systemP->debugPrint("Failed: ranout of address");//Debug only
					return false;
				}

				if (!sendRequest(toAddress, toUpperCase(command), forwardAddress))
				{
					return false;
				}
				else
				{
					systemP->debugPrint("true");
					digitalWrite(DEBUG_LED, LOW);
					radioP->txStandBy();
					numOfRetries = 0;
					return true;
				}
			}

			systemP->errorReport(3, toAddress);
			digitalWrite(DEBUG_LED, LOW);
			numOfRetries = 0;
			return false;
		}
		else
		{
			//systemP->errorReport(3, toAddress);
			systemP->debugPrint("Failed");
			digitalWrite(DEBUG_LED, LOW);
			numOfRetries = 0;
			return false;
		}
	}
}

char Radio::waitForRequest()
{
	char command;
	char codedMessageCStr[MESSAGE_SIZE] = {};
	String codedMessage;
	int fromAddress;
	int toAddress;
	bool breakLoop = false;

	radioP->openReadingPipe(0, myAddress); //must be set to receiver's address. Will only receive data from hubs that have opened writing pipes to this address
	radioP->startListening();

	//Stay in a loop, because the request might not be for us
	while (!breakLoop)
	{
		//radioP->openReadingPipe(0, myAddress); //must be set to receiver's address. Will only receive data from hubs that have opened writing pipes to this address
		//radioP->startListening();

		systemP->debugPrint("Waiting for request...");
		waitForData(-1);
		systemP->debugPrint("Got...");

		radioP->read(codedMessageCStr, MESSAGE_SIZE);

		codedMessage = codedMessageCStr;

		systemP->debugPrint(codedMessage);//Debug

		decodeMessage(fromAddress, command, toAddress, codedMessage);

		//If the request is for us
		if (toAddress == myAddress)
		{
			//leave the loop
			breakLoop = true;
		}
		else
		{
			//Forward the request
			systemP->debugPrint("Forwarding...");
			sendRequest(toAddress, 'D', 0, fromAddress);
			
			systemP->debugPrint("Done forwarding");//Debug only
			
			radioP->openReadingPipe(0, myAddress); //must be set to receiver's address. Will only receive data from hubs that have opened writing pipes to this address
			radioP->startListening();
		}
	}

	return command;
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
				systemP->debugPrint("Successful contact with hub ");//debug only
				systemP->debugPrint(debug);//debug only
			}
		}

	}
	else
	{
		systemP->debugPrint("Waiting for pings...");//debug only
		pingsSuccess = receivePing();
	}

	return pingsSuccess;
}

String Radio::encodeMessage(float temperature, int motion, int fromAddress, int toAddress)    /// TODO: Add toAddress to the package
{
	/*
	Follows the following format:

	fromAddress -> spacer -> temperature -> spacer -> motion -> spacer -> toAddress -> end char

	Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
	Array index slot -->[9,0,0,1,-,7,4,.,5,6,-, 5, 8, 9, -, 9, 0, 0, 3, x]

	9001-74.36-589-9003x
	*/


	/*systemP->debugPrint(temperature); //Debug only
	systemP->debugPrint(motion);//Debug only
	systemP->debugPrint(fromAddress);//Debug only
	systemP->debugPrint(toAddress); //Debug only*/

	if ((fromAddress < 9001 || fromAddress > 9999) || (motion < 0 || motion > 9999) || (temperature < 0 || temperature > 9999) || (toAddress < 9001 || toAddress > 9999))
	{
		return "ERROR";
	}

	char message[MESSAGE_SIZE] = {};
	String numberHolder;
	String messageToReturn;
	int lastSpacePos = 0; //posistion of last spacer

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

	message[numberHolder.length() + 11] = '-';
	lastSpacePos = (numberHolder.length() + 11);

	//Add toAddress into the cstring
	numberHolder = static_cast<String>(toAddress);

	for (int i = 0; i < numberHolder.length(); i++)
	{
		message[i + (lastSpacePos + 1)] = numberHolder[i];
	}

	//Add the end char
	message[numberHolder.length() + (lastSpacePos + 1)] = 'x';

	messageToReturn = message;

	systemP->debugPrint(message);//debug only

	return messageToReturn;
}

String Radio::encodeMessage(char command, int toAddress, int fromAddress)
{
	/*
	Follows the following format:

	fromAddress -> spacer -> command -> spacer -> toAddress -> end char

	Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11
	Array index slot -->[9,0,0,1,-,S,-,9,0,0, 3, x]

	9001-S-9003x
	*/

	if ((fromAddress < 9001 || fromAddress > 9999) || (toAddress < 9001 || toAddress > 9999))
	{
		return "ERROR";
	}

	String numberHolder;
	String messageToReturn;
	int lastSpacePos = 0; //posistion of last spacer
	char message[MESSAGE_SIZE] = {};

	numberHolder = static_cast<String>(fromAddress);

	//Add the fromAddress into the cstring
	for (int i = 0; i < 4; i++)
	{
		message[i] = numberHolder[i];
	}

	message[4] = '-';

	//Add the command char
	message[5] = toUpperCase(command);

	message[6] = '-';

	numberHolder = static_cast<String>(toAddress);

	//Add the toAddress into the cstring
	for (int i = 0; i < 4; i++)
	{
		message[i + 7] = numberHolder[i];
	}

	//Add the end char
	message[11] = 'x';

	messageToReturn = message;

	systemP->debugPrint(messageToReturn);//Debug only

	return messageToReturn;
}





bool Radio::decodeMessage(float &temperature, int &motion, int &fromAddress, int &toAddress, const String &codedMessage)
{
	String stringHolder;
	int indx = 0;
	//int secondIndx = 0;
	char messageData[5];
	char message[MESSAGE_SIZE] = {};

	strcpy(message, codedMessage.c_str());

	if (isValidMessage(codedMessage) == false)
	{
		systemP->errorReport(11);
		return false;
	}


	//Get fromAddress data
	while (message[indx] != '-')
	{
		systemP->debugPrint(message[indx]);
		stringHolder += message[indx];
		indx++;
	}

	fromAddress = atoi(stringHolder.c_str()); //Convert string into int
	systemP->debugPrint(fromAddress);//debug only
	indx++;

	stringHolder = "";

	//Get temperature data
	while (message[indx] != '-')
	{
		stringHolder += message[indx];
		indx++;
		//secondIndx++;
	}

	temperature = atof(stringHolder.c_str()); //Convert string into int

	indx++;
	//secondIndx = 0;

	stringHolder = "";

	//Get motion data
	while (message[indx] != '-')
	{
		stringHolder += message[indx];
		indx++;
		//secondIndx++;
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

bool Radio::decodeMessage(int & fromAddress, char & command, int & toAddress, const String & codedMessage)
{
	String stringHolder;
	int indx = 0;
	//int secondIndx = 0;
	char messageData[5];
	char message[MESSAGE_SIZE] = {};

	strcpy(message, codedMessage.c_str());

	if (isValidMessage(codedMessage, false) == false)
	{
		systemP->errorReport(11);
		return false;
	}

	//Get fromAddress data
	while (message[indx] != '-')
	{
		stringHolder += message[indx];
		indx++;
	}

	fromAddress = atoi(stringHolder.c_str()); //Convert string into int

	indx++;
	stringHolder = "";

	//Get the command
	command = message[indx];

	indx += 2;

	//Get toAddress data
	while (message[indx] != 'x')
	{
		stringHolder += message[indx];
		indx++;
	}

	toAddress = atoi(stringHolder.c_str());

	return true;
}

bool Radio::waitForData(int milliSec, bool printError)
{
	int targetTime = 0;

	if (milliSec >= 0)
	{
		targetTime = millis() + milliSec;
		while (!radioP->available() && millis() < targetTime) {}

		if (millis() > targetTime)
		{
			//Only print the error if we are told to
			if (printError = true)
			{
				systemP->errorReport(10);
			}
			return false;
		}
	}
	else
	{
		while (!radioP->available()) {}
	}

	return true;
}

bool Radio::isValidMessage(const String &codedMessage, bool isLargeMessage)
{
	unsigned int spacerCount = 0;
	unsigned int maxNumOfSpacers = 0;

	systemP->debugPrint(codedMessage);

	if (isLargeMessage)
	{
		maxNumOfSpacers = 4;
	}
	else
	{
		maxNumOfSpacers = 3;
	}

	//count the spacers ( - ) and end char ( x )
	for (int indx = 0; indx < codedMessage.length(); indx++)
	{
		if (codedMessage[indx] == '-' || codedMessage[indx] == 'x')
		{
			spacerCount++;
		}
	}

	//If message was formated correctly, the variable should equal 4 or 3
	if (spacerCount == maxNumOfSpacers)
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
		//targetTime = millis() + 30000;
		//while (!radioP->available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 30sec
		//waitForData(-1);

		if (waitForData(-1))
		{
			systemP->debugPrint("Receiving Data..."); //debug only
			radioP->read(&toAddress, sizeof(toAddress));
		}
		/*else if (millis() > targetTime)
		{
			systemP->debugPrint("ERROR!!!");
			systemP->errorReport(9);
		}*/

		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Receive the "fromAddress" //////////////////////////
		//targetTime = millis() + 30000;
		//while (!radioP->available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 30sec
		//waitForData(30000);

		if (waitForData(30000, false))
		{
			radioP->read(&fromAddress, sizeof(fromAddress));
		}
		else
		{
			systemP->errorReport(9);
		}

		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Make Sure we received the right address ///////////////////
		if (toAddress == myAddress)
		{
			radioP->stopListening();
			radioP->openWritingPipe(range[0]);

			systemP->debugPrint("Sending Ping back..."); //debug only

			if (radioP->write(&myAddress, sizeof(myAddress)))
			{
				pingReceive = true;
			}
			else
			{
				systemP->debugPrint("ERROR!!!");
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
	bool didSend = false;
	numOfRetries = 0;

	systemP->debugPrint(" ");//debug only
	systemP->debugPrint("Sending pings...");//debug only

	radioP->stopListening();
	radioP->openWritingPipe(range[index]);

	//\\\\\\\\\\\\\\\\\\\\\\\\\ Send "To Address" //////////////////////////

	toAddress = range[index];

	if (!radioP->write(&toAddress, sizeof(toAddress)))
	{
		while (numOfRetries < MAX_NUM_OF_RETRIES)
		{
			if (radioP->write(&toAddress, sizeof(toAddress)))
			{
				didSend = true;
				numOfRetries = 0;
				break;
			}
			delay(300);
			numOfRetries++;
		}

		if (didSend == false && numOfRetries)
		{
			systemP->errorReport(3, toAddress);
			numOfRetries = 0;
			return false;
		}
	}

	didSend = false;
	delay(200);

	//\\\\\\\\\\\\\\\\\\\\\\\\ Send "from address" ////////////////////////
	if (!radioP->write(&myAddress, sizeof(myAddress)))
	{
		while (numOfRetries < MAX_NUM_OF_RETRIES)
		{
			if (radioP->write(&myAddress, sizeof(myAddress)))
			{
				didSend = true;
				numOfRetries = 0;
				break;
			}
			delay(300);
			numOfRetries++;
		}

		if (didSend == false)
		{
			systemP->errorReport(3, toAddress);
			numOfRetries = 0;
			return false;
		}
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

	//targetTime = millis() + 1000;

	waitForData(1000);

	//while (!radioP->available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 1sec

	if (radioP->available())
	{
		radioP->read(&data, sizeof(data));

		if (data == acknowledgerAddress)
		{
			systemP->debugPrint("Received acknowledge...");//debug only
			didAcknowledge = true;
		}
		else
		{
			systemP->debugPrint("ERROR!!! Incorrect acknowledgement ping recieved");
			systemP->errorReport(8, acknowledgerAddress); //Report that we got the wrong data back
			didAcknowledge = false;
		}
	}
	else if (millis() > targetTime)///we went over time, report it
	{
		systemP->debugPrint("ERROR!!! No acknowledgement ping recieved");
		systemP->errorReport(7, acknowledgerAddress);
		didAcknowledge = false;
	}

	return didAcknowledge;
}