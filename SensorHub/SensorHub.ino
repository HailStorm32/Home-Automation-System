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

//Timer consts
const int SEND_TIME = 10000; //Time it takes a hub to completly send a message
const int BASE_CYCLE_TIME = 6000; //Time between data transmissions (in milliseconds) //Base time will be multiplied by each stations number so each hub will send data at different times 
const int READ_CYCLE = 3500;  //Time between reading the temp sensor (in milliseconds)  1000 = 1sec

#include "Init_Functions.h"

const int MY_ADDRESS = setAddress();
const int RANGE[5] = { giveRange(1, MY_ADDRESS), giveRange(2, MY_ADDRESS), giveRange(3, MY_ADDRESS), giveRange(4, MY_ADDRESS), giveRange(5, MY_ADDRESS) };

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

	radio.setRetries(15, 15);
}

void loop()
{
	//Setup a variable that will hold the temperature & motion of each hub
	//There are a total of 8 (0-7) to hold data from each hub
	int hubTemperature[8] = {};
	int hubMotion[8] = {};
	int cycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MY_ADDRESS - 9000)));//Time between data transmissions

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

	radio.openReadingPipe(0, RANGE[0]);
	radio.openReadingPipe(1, RANGE[1]);
	radio.openReadingPipe(2, RANGE[2]);
	radio.openReadingPipe(3, RANGE[3]);
	radio.openReadingPipe(4, RANGE[4]);
	radio.openReadingPipe(5, RANGE[5]);

	radio.startListening();

	///////////////////////Begin "Main Code"\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	
	int MotionCount = 0;
	float TemperatureCount = 0;


	///////-VARIABLE DECLARATIONS-//////// 
	int MathHolder = 0;
	int counter = 0;
	bool PIRisNotConected = true;
	int PastTime1 = millis();
	int PastTime2 = PastTime1;

	delay(10000);
	dataSend(9001, 'T', 66, 5, "nul");

	while (true)
	{
		if (radio.available())
		{
			receiveData(hubTemperature, hubMotion);

			if (MY_ADDRESS == 9001)
			{
				Serial.println("Temperature Data: ");
				for (int i = 0; i < 8; i++)
				{
					Serial.println(hubTemperature[i]);
				}

				Serial.println("--------------------");

				Serial.println("Motion Data: ");
				for (int i = 0; i < 8; i++)
				{
					Serial.println(hubMotion[i]);
				}
			}
		}
	}
}