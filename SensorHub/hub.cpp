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

Hub::Hub()
{
	temperature = 0;
	motion = 0;
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
		Serial.println("");
		Serial.println("ERROR 001!! Station address not set!");
		Serial.println("Please restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(300);
			digitalWrite(ERROR_LED, LOW);
			delay(300);
		}

		return;

	case 2:
		Serial.println("");
		Serial.println("ERROR 002!! PIR sensor not found!");
		Serial.println("Please check connection and restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(500);
			digitalWrite(ERROR_LED, LOW);
			delay(500);
		}

		return;

	case 3:
		Serial.println(" ");
		Serial.print("ERROR 003!! Unable to contact hub ");
		Serial.print(address);
		Serial.println("!!");
		//Serial.println("Please restart!");

		//while (true) //Blink error LED
		//	{
		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
		//delay(700);
		//}

		return;

	case 4:
		Serial.println(" ");
		Serial.print("ERROR 004!! Issue with code");
		Serial.println("!!");
		Serial.println("Please restart!");

		while (true)
		{
			digitalWrite(ERROR_LED, HIGH);
		}

		return;

	case 5:
		Serial.println(" ");
		Serial.print("ERROR 005!! Station ");
		Serial.print(address);
		Serial.println(" did not set message type!!");
		Serial.println("Please restart!");

		//	while (true)
		//{
		digitalWrite(ERROR_LED, HIGH);
		delay(800);
		digitalWrite(ERROR_LED, LOW);
		delay(800);
		//}

		return;

	case 6:
		Serial.println(" ");
		Serial.print("ERROR 006!! Failed to read from DHT sensor!");
		Serial.println("!!");
		Serial.println("Please restart!");

		while (true)
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(200);
			digitalWrite(ERROR_LED, LOW);
			delay(200);
		}
		
		return;

	case 7:
		Serial.println(" ");
		Serial.print("ERROR 007!! No acknowledgement ping recieved from ");
		Serial.print(address);
		Serial.println("!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 8:
		Serial.println(" ");
		Serial.print("ERROR 008!! Incorrect acknowledgement ping recieved from ");
		Serial.print(address);
		Serial.println("!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 9:
		Serial.println(" ");
		Serial.println("ERROR 009!! Time out! No ping received!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 10:
		Serial.println(" ");
		Serial.println("ERROR 010!! Time out! No data received!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);

		return;

	case 11:
		Serial.println(" ");
		Serial.println("ERROR 011!! Mesage not formated!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(2000);
		digitalWrite(ERROR_LED, LOW);

		return;
	}
}