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

Hub hubSystem;

Radio mainRadio(MY_ADDRESS, RANGE, &radio, &hubSystem);

bool pingStatus = false;//debug only



void setup()
{
	Serial.begin(9600);

	//Serial.println("in setup");

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

	int targetTime = millis() + 2000;
	bool doPings = true;
	bool dataRecived = false;
	String request = "";

	Serial.flush();

	//Get info from the command center on if we should do pings
	while(!dataRecived)
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
	}

	if (doPings)
	{
		pingStatus = mainRadio.startupPings();
	}
	else
	{
		pingStatus = false;
	}

	digitalWrite(DEBUG_LED, LOW); //Show that we have exited the "setup" stage

	//radio.setRetries(15, 15);


	//radio.openReadingPipe(0, 9001);
	//radio.openReadingPipe(1, RANGE[1]);
	//radio.openReadingPipe(2, RANGE[2]);
	//radio.openReadingPipe(3, RANGE[3]);
	//radio.openReadingPipe(4, RANGE[4]);
	//radio.openReadingPipe(5, RANGE[5]);

	//radio.startListening();

	///Debug StuffVVV
	//Serial.print("Ping status: ");
	//Serial.println(pingStatus);

	//Serial.println("/////////////////////");
	//Serial.print("My Address: ");
	//Serial.println(MY_ADDRESS);
	//Serial.println("Read Range:");
	for (int i = 0; i <= 4; i++)
	{
		//Serial.println(RANGE[i]);
	}
}

void loop()
{
	String request;

	//Serial.println("VVVV");
	////Serial.println(codedMessage2);
	//Serial.println("___________________");

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
				Serial.print(mainRadio.encodeMessage(dht.convertCtoF(dht.readTemperature()), 4, MY_ADDRESS));

				return;
			case 9002:
				if (mainRadio.requestData(9002))
				{
					//Serial.println("9002:");
					mainRadio.receiveData();
					//Serial.println("___________________");
					//Serial.println(" ");
				}
				return;
			case 9003:
				if (mainRadio.requestData(9003))
				{
					//Serial.println("9003:");
					mainRadio.receiveData();
					//Serial.println("___________________");
					//Serial.println(" ");
				}
				return;
			}
		}
		//IF not master hub
		else
		{
			mainRadio.waitForRequest();

			mainRadio.sendData(dht.convertCtoF(dht.readTemperature()), 5, MY_ADDRESS, 9001);
			
		}

		request = "";
	}


}