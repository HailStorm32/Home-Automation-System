//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are licensed by their respective owners		//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//

//#include "constants.h"
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

Radio mainRadio(MY_ADDRESS, RANGE, &radio);

bool pingStatus = false;//debug only

Hub hub1; 

void setup()
{
	Serial.begin(9600);

	Serial.println("in setup");

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

	pingStatus = mainRadio.startupPings();

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
}

void loop()
{
	
	//codedMessage2 = data.encodeMessage(10, 0, 9001);

	Serial.println("VVVV");
	//Serial.println(codedMessage2);
	Serial.println("___________________");

	//data.decodeMessage(temp, motion, fromAdd, codedMessage2);

	//Serial.println(temp);
	//Serial.println(motion);
	//Serial.println(fromAdd);

	while (true)
	{

	}

	if (MY_ADDRESS == 9001)
	{
		///dataReceive(debug);

		//Serial.println(debug);
		Serial.println("----------");
	}

	//For debug VVV
	if (MY_ADDRESS == 9003)
	{
		//delay(2500);
		//dataSend(data, 9001);

	}//^^^


	if (MY_ADDRESS == 9002)
	{
		//delay(2000);

		//dataSend(data2, 9001);

	}


}