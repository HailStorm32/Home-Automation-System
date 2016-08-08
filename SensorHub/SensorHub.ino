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


int MyAddress = 9999; //Set up station's personal address. Set to 9999 to catch errors
int MotionCount = 0; //Setup a variable that will hold the number of times the PIR sensor was activated
unsigned long time = 0; //Setup variable that will cycle when to sensor readings at timed intervals
int CycleTime = 6000; //Time between data transmissions (in milliseconds)  1000 = 1sec

bool flag = true;

int message = 0; //Setup variable that will hold the incoming message data
float temperature = 0; //Setup variable that will hold the incoming message data




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
					SendNum(1, MotionCount, 5, Range[0]);
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
			
			MotionCount = MotionCount + 1; //Add on to the # of motion captures 
		}

		line135:

		//See if there are any incoming messages  
		if (radio.available())
		{
			//Record message and print it
			radio.read(&message, sizeof(message));
			
			//Serial.println(" ");
			//Serial.print(message);

			//Will only read the remaing message if its addressed to itsself
			if (message == MyAddress)
			{

					while(radio.available() != true){} //Wait until a signal is available

					radio.read(&message, sizeof(message));//Read data
					
					//Serial.println(" ");
					//Serial.print(message);

					//If this is the end of the message, end. 
					if (message == 999)
					{
						Serial.println(" "); //Debug Only
						Serial.print("ENDing"); //Debug Only
						break;
					}

					//If not, write the data to the varable that holds Motion data
					else
					{
						MotionCount = message;
					}

					while (radio.available() != true) {} //Wait until a signal is available
					
					radio.read(&message, sizeof(message)); //Read data

					//Serial.println(" ");
					//Serial.print(message);

					//If this is the end of the message, end.
					if (message == 999)
					{
						Serial.println(" "); //Debug Only
						Serial.print("ENDing"); //Debug Only
						break;
					}
					
					//If not, write the data to the varable that holds Temperature data
					else
					{
						temperature = message;
					}		

				Serial.println(" ");  //Debug Only
				Serial.print("END");  //Debug Only

				Serial.println(" ");
				Serial.println(MotionCount);
				Serial.println(temperature);

			}

		}

	}

	Error(4);

}
