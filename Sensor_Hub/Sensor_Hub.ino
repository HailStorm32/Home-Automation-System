#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(7, 8);

//Set the addresses for all stations
const int Station1 = 9001;
const int Station2 = 9002;
const int Station3 = 9003;
const int Station4 = 9004;
const byte errorLED = 3;

int MyAddress = 9999; //Set up station's personal address. Set to 9999 to catch errors



void setup()
{
  Serial.begin(9600);

  radio.begin();

  pinMode(2, INPUT);
  pinMode(errorLED, OUTPUT);

///////////SET ADDRESS/////////////////////////////
  if (digitalRead(2) == LOW)
  {
    Serial.println("Address #1:");

    MyAddress = Station1;

    Serial.println(MyAddress);
  }

  else if (digitalRead(2) == HIGH)
  {
    Serial.println("Address set to: #2");

    MyAddress = Station2;

    Serial.println(MyAddress);
  }
///////////SET ADDRESS/////////////////////////////


  //Make sure station was set correctly
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



  
}

void loop()
{


}
