//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are coppyrighted by their respective owners	//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//

#include <SPI.h>
#include "nRF24L01.h"
#include <RF24.h>
#include <DHT.h>

#define DHTPIN 6     // Define what digital pin the temp sensor is connected to

#define DHTTYPE DHT11  //Define the type of temp sensor we are useing

RF24 radio(9, 10);

DHT dht(DHTPIN, DHTTYPE); // Initialize temp sensor

//Set the addresses for all stations
const int Station[8] = { 9001, 9002, 9003, 9004, 9005, 9006, 9007, 9008 };

int *Range = new int[5];

//Variables to hold this stations senser data
int MotionCount = 0;
int TemperatureCount = 0;

//Set Arduino pins
const byte switchIn_1 = 2;
const byte switchIn_2 = 3;
const byte switchIn_3 = 4;
const byte errorLED = 7;
const byte PIR = 5;

int MyAddress = 9999; //Set up station's personal address. Set to 9999 to catch errors

//Setup variables that will hold time data
unsigned long time1 = 0; 
unsigned long time2 = 0;

const int SendTime = 10000; //Time it takes a hub to completly send a message
const int BaseCycleTime = 6000; //Time between data transmissions (in milliseconds) //Base time will be multiplied by each stations number so each hub will send data at different times 
const int ReadCyle = 3500;  //Time between reading the temp sensor (in milliseconds)  1000 = 1sec
int CycleTime = 0;  //Time between data transmissions

bool flag = true;


//Value holders VV ///

//Setup a variable that will hold the temperature
//There are a total of 8 to hold data from each hub
int temperature_01 = 0;
int temperature_02 = 0;
int temperature_03 = 0;
int temperature_04 = 0;
int temperature_05 = 0;
int temperature_06 = 0;
int temperature_07 = 0;
int temperature_08 = 0;

//Setup a variable that will hold the humidity
//There are a total of 8 to hold data from each hub
/*int humidity_01 = 0;
int humidity_02 = 0;
int humidity_03 = 0;
int humidity_04 = 0;      //TO BE ADDED IN LATER
int humidity_05 = 0;
int humidity_06 = 0;
int humidity_07 = 0;
int humidity_08 = 0;  */

//Setup a variable that will hold the number of times the PIR sensor was activated
//There are a total of 8 to hold data from each hub
int MotionCount_01 = 0;
int MotionCount_02 = 0;
int MotionCount_03 = 0;
int MotionCount_04 = 0;
int MotionCount_05 = 0;
int MotionCount_06 = 0;
int MotionCount_07 = 0;
int MotionCount_08 = 0;



#include "Functions.h"

void setup()
{
	Serial.begin(9600);

	radio.begin();
	delay(200);
	dht.begin();

	//Setup pins
	pinMode(switchIn_1, INPUT);
	pinMode(switchIn_2, INPUT);
	pinMode(switchIn_3, INPUT);
	pinMode(PIR, INPUT);
	pinMode(errorLED, OUTPUT);
	pinMode(8, OUTPUT);

	digitalWrite(8, HIGH);

	SetAddress();

	//Make sure station address was set correctly
	if (MyAddress == 9999 || MyAddress < 8999) //If the variable for this station's address is still the default value, give an error
	{
		Error(1);
	}
	else
	{
		Serial.println("Address is: ");
		Serial.println(MyAddress);  //Print the hub's address
	}

	SetReadRange(); //Set which stations this current station can receive from based on its address

	delay(1000); //Delay to allow for buffer time
	
	FirstPing();//Have the master station ping all other stations 

	//Set the reading pipes
	radio.openReadingPipe(0, Range[0]);
	radio.openReadingPipe(1, Range[1]);
	radio.openReadingPipe(2, Range[2]);
	radio.openReadingPipe(3, Range[3]);
	radio.openReadingPipe(4, Range[4]);
	//radio.openReadingPipe(5, Range[5]);

	radio.startListening();

}

void loop()
{
	digitalWrite(8, LOW);
	int holder = 0;
	int counter = 0;
	//bool flag2 = false;
	time1 = millis();
	time2 = time1;
	
	while (true)
	{
		
		//Send the data to the master hub(or closest hub) at timed intervals defined by the CycleTime variable
		//....Hope to add a clock in the future to help keep better timing
		if (millis() > time1 + CycleTime)
		{
				
				flag = false; //Set the error flag to false

				//delay(200 + (1000 * (MyAddress - 9000)));

				if (MyAddress != 9001)
				{

					TemperatureCount = TemperatureCount / counter; //average the readings from the DHT
					
					SendData(9001, MotionCount, TemperatureCount, "", 3); //send the data

					//Reset variables
					TemperatureCount = 0;
					MotionCount = 0;
					counter = 0;

				}

				else if (MyAddress == 9001)
				{
					TemperatureCount = TemperatureCount / counter; //average the readings from the DHT

					temperature_01 = TemperatureCount;
					MotionCount_01 = MotionCount;

					TemperatureCount = 0;
					MotionCount = 0;

				}
					

				//Reset the timer
				time1 = millis();
		}
	

		//See if there is motion in the room and record it
		if (digitalRead(PIR) == HIGH)
		{
			
			if (flag == true)//Check to see if PIR sensor is conected
			{ 
				//Error(2);  //UNCOMMENT AFTER DEBUG!!!
			}
			
			delay(2500); //Because the sensor ouputs a long HIGH for a single trigger, the delay is to prevent spammed adding (hope to be fixed soon)
			
			Serial.println("MOTION");//For Debuging ONLY
			
			MotionCount = MotionCount + 1; //Add on to the # of motion captures 
		}

		
		//Read data from the DHT sensor at timed intervals defiend by the ReadCycle variable
		if (millis() > time2 + ReadCyle)
		{
			holder = dht.readTemperature(true);
			
			TemperatureCount = TemperatureCount + holder;

			if (isnan(holder)) 
			{
				Error(6);
			}

			counter++;

			time2 = millis();
		}

		//See if there are any incoming messages  
		
		if (radio.available())
		{
			
			DataReceive();

			if (MyAddress == 9001)
			{
				
				Serial.println("  ");
				Serial.print("9001 -- Motion: ");
				Serial.println(MotionCount_01);
				Serial.print("9001 -- Temperature: ");
				Serial.println(temperature_01);

				Serial.println("  ");
				Serial.print("9002 -- Motion: ");
				Serial.println(MotionCount_02);
				Serial.print("9002 -- Temperature: ");
				Serial.println(temperature_02);

				Serial.println("  ");
				Serial.print("9003 -- Motion: ");
				Serial.println(MotionCount_03);
				Serial.print("9003 -- Temperature: ");
				Serial.println(temperature_03);

			/*	Serial.print("9004 -- Motion: ");
				Serial.println(MotionCount_04);
				Serial.print("9004 -- Temperature: ");
				Serial.println(temperature_04);

				Serial.print("9005 -- Motion: ");
				Serial.println(MotionCount_05);
				Serial.print("9005 -- Temperature: ");
				Serial.println(temperature_05);

				Serial.print("9006 -- Motion: ");
				Serial.println(MotionCount_06);
				Serial.print("9006 -- Temperature: ");
				Serial.println(temperature_06); */
			}
		}

	}

	Error(4);

}
