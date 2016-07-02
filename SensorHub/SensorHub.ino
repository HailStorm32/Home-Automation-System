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

//variables that will hold the addresses that the hub will listen for (has a max of 6)
int Range[5] = {};

//Set Arduino pins
const byte switchIn_1 = 2;
const byte switchIn_2 = 3;
const byte switchIn_3 = 4;
const byte errorLED = 7;
const byte PIR = 5;


int MyAddress = 9999; //Set up station's personal address. Set to 9999 to catch errors

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
	
}
