//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are licensed by their respective owners		//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//

#include "hub.h"

Hub::Hub(bool debugMode)
{
	temperature = 0;
	motion = 0;
	this->debugMode = debugMode;
}

bool Hub::logTemperature(float data)
{
	return false;
}

bool Hub::logMotion(int data)
{
	return false;
}

float Hub::getTemerature() const
{
	return 0;
}

float Hub::getMotion() const
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Error Codes:
//		01 -- Station address not set
//		02 -- PIR sensor not found
//		03 -- Unable to contact [****] hub
//		04 -- Issue with code
//		05 -- Station [****] didnt send message type
//		06 -- Failed to read from DHT sensor
//		07 -- No acknowledgement ping recieved from [****]
//		08 -- Incorrect acknowledgement ping recieved from [****]
//		09 -- Time out, No ping received
//		10 -- Time out, No data received
//		11 -- Message not formated 
//
// Arguments:
//		 errorCode (I) -- integer # between 1-10
//		 address (I) --  address of receiver (optional)
///////////////////////////////////////////////////////////////////////////////////////////////////
void Hub::errorReport(int errorCode, int address)
{
	Serial.begin(9600);
	
	switch (errorCode)
	{
	case 1:
		debugPrint("");
		debugPrint("ERROR 001!! Station address not set!");
		debugPrint("Please restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(300);
			digitalWrite(ERROR_LED, LOW);
			delay(300);
		}

		return;

	case 2:
		debugPrint("");
		debugPrint("ERROR 002!! PIR sensor not found!");
		debugPrint("Please check connection and restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(500);
			digitalWrite(ERROR_LED, LOW);
			delay(500);
		}

		return;

	case 3:			
		debugPrint(" ");
		debugPrint("ERROR 003!! Unable to contact hub ");
		debugPrint(address);
		debugPrint("!!");
		//debugPrint("Please restart!");

		//while (true) //Blink error LED
		//	{
		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
		//delay(700);
		//}
		return;

	case 4:
		debugPrint(" ");
		debugPrint("ERROR 004!! Issue with code");
		debugPrint("!!");
		debugPrint("Please restart!");

		while (true)
		{
			digitalWrite(ERROR_LED, HIGH);
		}

		return;

	case 5:
		debugPrint(" ");
		debugPrint("ERROR 005!! Station ");
		debugPrint(address);
		debugPrint(" did not set message type!!");
		debugPrint("Please restart!");

		//	while (true)
		//{
		digitalWrite(ERROR_LED, HIGH);
		delay(800);
		digitalWrite(ERROR_LED, LOW);
		delay(800);
		//}

		return;

	case 6:
		debugPrint(" ");
		debugPrint("ERROR 006!! Failed to read from DHT sensor!");
		debugPrint("!!");
		debugPrint("Please restart!");

		while (true)
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(200);
			digitalWrite(ERROR_LED, LOW);
			delay(200);
		}
		
		return;

	case 7:
		debugPrint(" ");
		debugPrint("ERROR 007!! No acknowledgement ping recieved from ");
		debugPrint(address);
		debugPrint("!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 8:
		debugPrint(" ");
		debugPrint("ERROR 008!! Incorrect acknowledgement ping recieved from ");
		debugPrint(address);
		debugPrint("!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 9:
		debugPrint(" ");
		debugPrint("ERROR 009!! Time out! No ping received!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 10:
		debugPrint(" ");
		debugPrint("ERROR 010!! Time out! No data received!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 11:
		debugPrint(" ");
		debugPrint("ERROR 011!! Mesage not formated!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(2000);
		digitalWrite(ERROR_LED, LOW);

		return;
	}
}

void Hub::debugPrint(String message)
{
	//If debugging has been enabled
	if (debugMode == true)
	{
		Serial.println(message);
	}
}

void Hub::debugPrint(float data)
{
	//If debugging has been enabled
	if (debugMode == true)
	{
		Serial.println(data);
	}
}
