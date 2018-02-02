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
#include <DHT.h>
#include "hub.h"
#include "radio.h"
#include "Init_Functions.h"

#define DHTPIN 6     // Define what digital pin the temp sensor is connected to
#define DHTTYPE DHT11  //Define the type of temp sensor we are useing

RF24 radio(9, 10); //Set what pins the radio are in

DHT dht(DHTPIN, DHTTYPE); // Initialize temp sensor

const int MY_ADDRESS = setAddress();
const int RANGE[5] = { giveRange(1, MY_ADDRESS), giveRange(2, MY_ADDRESS), giveRange(3, MY_ADDRESS), giveRange(4, MY_ADDRESS), giveRange(5, MY_ADDRESS) };

Hub hubSystem(DEBUG_MODE);

Radio mainRadio(MY_ADDRESS, RANGE, &radio, &hubSystem);

bool pingStatus = false;//debug only



void setup()
{
	Serial.begin(9600); 

	hubSystem.debugPrint("in setup");

	//Startup radio and temperature sensors
	radio.begin();
	delay(200);
	dht.begin();

	//Set pin modes
	pinMode(SWITCH_IN_1, INPUT);
	pinMode(SWITCH_IN_2, INPUT);
	pinMode(SWITCH_IN_3, INPUT);
	pinMode(PIR, INPUT);
	pinMode(ERROR_LED, OUTPUT);
	pinMode(DEBUG_LED, OUTPUT);

	digitalWrite(DEBUG_LED, HIGH); //Show that we have entered the "setup" stage

	//Radio setup stuff
	radio.setPALevel(RF24_PA_MAX);
	if (!radio.setDataRate(RF24_250KBPS))
	{
		hubSystem.debugPrint("FAILED TO SET DATA RATE!!!");
	}
	radio.setChannel(124); 

	int targetTime = millis() + 2000;
	bool doPings = true;
	bool dataRecived = false;
	String request = "";

	Serial.flush();

	//Get info from the command center on if we should do pings
	/*while(!dataRecived)
	{
		//We have gone 2sec with no instructions from the command center, do pings anyway
		if (millis() >= targetTime)
		{
			//digitalWrite(ERROR_LED, HIGH);
			doPings = true;
			dataRecived = false;
			break;
		}

		if (Serial.available() > 0)
		{
			request = Serial.readString();
			
			if (request == "true")
			{
				doPings = true;
			}
			else if (request == "false")
			{
				doPings = false;
			}
			dataRecived = true;
		}
	}*/

	doPings = false;//Debug Only

	if (doPings)
	{
		pingStatus = mainRadio.startupPings();
	}
	else
	{
		pingStatus = false;
	}

	digitalWrite(DEBUG_LED, LOW); //Show that we have exited the "setup" stage

	///Debug StuffVVV
	hubSystem.debugPrint("Ping status: ");
	hubSystem.debugPrint(pingStatus);

	hubSystem.debugPrint("/////////////////////");
	hubSystem.debugPrint("My Address: ");
	hubSystem.debugPrint(MY_ADDRESS);
	hubSystem.debugPrint("Read Range:");
	for (int i = 0; i <= 4; i++)
	{
		hubSystem.debugPrint(RANGE[i]);
	}
}

void loop()
{
	String request;

	hubSystem.debugPrint("VVVV");
	//hubSystem.debugPrint(codedMessage2);
	hubSystem.debugPrint("___________________");

	float temperature = 0;
	int motion = 0;
	int fromAddress = 0;
	int toAddress = 0;
	char command = ' ';

	while (true)
	{
		if (MY_ADDRESS == 9001)
		{
			if (Serial.available() > 0)
			{
				request = Serial.readString();
			}

			switch (atoi(request.c_str()))
			{
			case 9001:
				Serial.print(mainRadio.encodeMessage(dht.convertCtoF(dht.readTemperature()), 4, MY_ADDRESS, 9001)); 

				return;
			case 9002:
				if (mainRadio.sendRequest(9002, 'S'))
				{
					hubSystem.debugPrint("9002:");
					mainRadio.receiveData();
					hubSystem.debugPrint("___________________");
					hubSystem.debugPrint(" ");
				}
				return;
			case 9003:
				if (mainRadio.sendRequest(9003, 'S'))
				{
					hubSystem.debugPrint("9003:");
					mainRadio.receiveData();
					hubSystem.debugPrint("___________________");
					hubSystem.debugPrint(" ");
				}
				return;
			}
		}
		//IF not master hub
		else
		{
			command = mainRadio.waitForRequest();

			if (command == 'S')
			{
				mainRadio.sendData(dht.convertCtoF(dht.readTemperature()), 4, 9001);
				command = ' ';
			}
			else if (command == 'F')
			{
				mainRadio.receiveData();
				command = ' ';
			}
			
		}

		request = "";
	}


}