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


#define DHTPIN 6     // Define what digital pin the temp sensor is connected to
#define DHTTYPE DHT11  //Define the type of temp sensor we are useing

RF24 radio(9, 10); //Set what pins the radio are in

DHT dht(DHTPIN, DHTTYPE); // Initialize temp sensor

const int STATION[8] = { 9001, 9002, 9003, 9004, 9005, 9006, 9007, 9008 }; //Set the addresses for all stations

//Set Arduino pins
const byte SWITCH_IN_1 = 2;
const byte SWITCH_IN_2 = 3;
const byte SWITCH_IN_3 = 4;
const byte ERROR_LED = 7;
const byte PIR = 5;
const byte DEBUG_LED = 8;

#include "Init_Functions.h"

const int MY_ADDRESS = setAddress();
const int RANGE[5] = { giveRange(1, MY_ADDRESS), giveRange(2, MY_ADDRESS), giveRange(3, MY_ADDRESS), giveRange(4, MY_ADDRESS), giveRange(5, MY_ADDRESS)};

#include "MainFunctions.h"

bool pingStatus = false;//debug only

void setup()
{	
	Serial.begin(9600);

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

	pingStatus = startupPings();

	digitalWrite(DEBUG_LED, LOW); //Show that we have exited the "setup" stage
}

void loop()
{
	
	Serial.print("Ping status: ");
	Serial.println(pingStatus);

	Serial.println("/////////////////////");
	Serial.print("My Address: ");
	Serial.println(MY_ADDRESS);
	Serial.println("Read Range:");
	for (int i = 0; i <= 4; i++)
	{
		Serial.println(RANGE[i]);
	}
	while (true)
	{
		 
	}
}