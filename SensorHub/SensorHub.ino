//**************************************************************************************//
//																						//
//			This code by Demetrius Van Sickle is licensed under a						//
//		Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																						//
//	The nRF24L01 libries used in this code are coppyrighted by their respective owners	//
//			and are issued under the GNU General Public License version 2.				//
//																					    //
//**************************************************************************************//

#include <SPI.h>
#include "nRF24L01.h"
#include <RF24.h>



RF24 radio(9, 10);

//Set the addresses for all stations
const int Station[8] = { 9001, 9002, 9003, 9004, 9005, 9006, 9007, 9008 };

int Range[5] = {}; //variables that will hold the addresses that the hub will listen for (has a max of 6)


//Set Arduino pins
const byte switchIn_1 = 2;
const byte switchIn_2 = 3;
const byte switchIn_3 = 4;
const byte errorLED = 7;
const byte PIR = 5;
//////////////////"0123456"
String S_Motion = "      M";
///////////////////////"0123456"
String S_Temperature = "      T";

int MyAddress = 9999; //Set up station's personal address. Set to 9999 to catch errors
 //Setup a variable that will hold the number of times the PIR sensor was activated
unsigned long time = 0; //Setup variable that will cycle when to sensor readings at timed intervals
int CycleTime = 6000; //Time between data transmissions (in milliseconds)  1000 = 1sec

bool flag = true;

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

	//Setup pins
	pinMode(switchIn_1, INPUT);
	pinMode(switchIn_2, INPUT);
	pinMode(switchIn_3, INPUT);
	pinMode(PIR, INPUT);
	pinMode(errorLED, OUTPUT);

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
	//Serial.println("ttttttttttttttttttttt");
	//Serial.println(S_Motion);

	T_Convert(66);
	M_Convert(233);

	time = millis();
	time = time + CycleTime;
	
	while (true)
	{
		
		//Send the data to the master hub(or closest hub) at timed intervals defined by the CycleTime variable
		//....Hope to add a clock in the future to help keep better timing
		if (millis() > time)
		{
				
				flag = false; //Set the error flag to false

				if (MyAddress != 9001)
				{
					
					//Serial.println("sdssdsdsdsdss");
					//Serial.println(S_Motion);
					
					SendNum(1, S_Motion, "", "", Range[0]);
				}

				time = millis();
				time = time + CycleTime;
		}
	

		//See if there is motion in the room 
		if (digitalRead(PIR) == HIGH)
		{
			
			if (flag == true)//Check to see if PIR sensor is conected
			{ 
				Error(2);
			}
			
			delay(2500); //Because the sensor ouputs a long HIGH for a single trigger, the delay is to prevent spammed adding (hope to be fixed soon)
			
			Serial.println("MOTION");//For Debuging ONLY
			
			//MotionCount = MotionCount + 1; //Add on to the # of motion captures 
		}


		//See if there are any incoming messages  
		if (radio.available())
		{
			
			DataReceive();

			Serial.println(" ");
			Serial.println(MotionCount_02);
			Serial.println(temperature_02);

		}

	}

	Error(4);

}
