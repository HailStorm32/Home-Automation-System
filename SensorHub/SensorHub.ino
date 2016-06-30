//**************************************************************************************//
//																						//
//			This code by Demetrius Van Sickle is licensed under a						//
//		Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																					    //
//**************************************************************************************//

#include <SPI.h>
#include "nRF24L01.h"
#include <RF24.h>



RF24 radio(9, 10);

//Set the addresses for all stations
const int Station1 = 9001;
const int Station2 = 9002;
const int Station3 = 9003;
const int Station4 = 9004;
const int Station5 = 9005;
const int Station6 = 9006;
const int Station7 = 9007;
const int Station8 = 9008;

const int Station[8] = { 9001, 9002, 9003, 9004, 9005, 9006, 9007, 9008 };

//variables that will hold the addresses that the hub will listen for (has a max of 6)
/*int Listen0 = 0;
int Listen1 = 0;
int Listen2 = 0;
int Listen3 = 0;
int Listen4 = 0;
int Listen5 = 0;*/

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
	
	//radio.setRetries(15, 15);

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

	//SetReadRange();
	
	FirstPing();

}

void loop()
{
	
}
