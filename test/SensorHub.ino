//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are licensed by their respective owners		//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//

#include "SPI.h"
#include <RF24.h>

RF24 radio(9, 10); //Set what pins the radio are in

byte addresses[32] = {};
char testing[20] = {"hello"};
byte defaultAddress = 2;

void setup()
{
    Serial.begin(9600);//For testing
    pinMode(3, INPUT);//For testing
    pinMode(8, OUTPUT);
    pinMode(7, OUTPUT);
    
    //Startup radio and temperature sensors
    radio.begin();
	delay(200);
    
    Serial.println("DDDD");

    radio.stopListening();
    radio.openWritingPipe(defaultAddress);


    randomSeed(analogRead(0));

    for(int indx = 0; indx  < 32; indx++)
    {
        addresses[indx] = random(2,255);
    }

    if(digitalRead(3) == LOW)
    {
        Serial.println("Sending");

        if(!radio.write(&addresses,32,0))
        {
            digitalWrite(7, HIGH);
            Serial.println("send failed");
        }
        else
        {
//            digitalWrite(7, LOW);
            Serial.println("Yay");
            while(true){}
        }
    }
}

void loop()
{

}
