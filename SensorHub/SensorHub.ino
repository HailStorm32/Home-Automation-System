#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


RF24 radio(7, 8);

//Set the addresses for all stations
const int Station1 = 9001;
const int Station2 = 9002;
const int Station3 = 9003;
const int Station4 = 9004;
const int Station5 = 9005;
const int Station6 = 9006;
const int Station7 = 9007;
const int Station8 = 9008;
const byte errorLED = 7;
const byte PIR = 5;


int MyAddress = 9999; //Set up station's personal address. Set to 9999 to catch errors

#include "Functions.h"

void setup()
{
	Serial.begin(9600);

	radio.begin();

	pinMode(switchIn_1, INPUT);
	pinMode(switchIn_2, INPUT);
	pinMode(switchIn_3, INPUT);
	pinMode(PIR, INPUT);
	pinMode(errorLED, OUTPUT);

	SetAddress();



	//Make sure station address was set correctly
	if (MyAddress == 9999) //If the variable for this station's address is still the default value, give an error
	{
		Serial.println("ERROR 001!! Station address not set!");
		Serial.print("Please restart!");

		while (true) //Blink error LED
		{
			digitalWrite(errorLED, HIGH);
			delay(500);
			digitalWrite(errorLED, LOW);
		}
	}


	else
	{
		Serial.println(MyAddress);
	}


}

void loop()
{


}