// Automatically generated by Makefile. Don't edit.
#include <Arduino.h>
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
char testing[32] = {"2-PR-55.3-2333-254&000000000000"};
byte defaultAddress = 2;
byte indx = 0;
byte myAddress = 254;
byte startIndx;
byte numOfSpacers = 0;
char packedReply[32] = {};
const byte ERROR_LED = 7;
const byte STATUS_LED = 8;
String command;
String message;
bool messageSent = false;
bool haveConnection = false;
bool receivedReply = false;

void setup()
{
    Serial.begin(9600);//For testing
    pinMode(3, INPUT);//For testing
    pinMode(2, INPUT);//For testing
    pinMode(8, OUTPUT);
    pinMode(7, OUTPUT);
    
    //Startup radio and temperature sensors
    radio.begin();
	delay(200);
    
    radio.setRetries(1, 15);
    radio.setChannel(124);
    //radio.setPALevel(RF24_PA_MIN);

    Serial.println("DDDD");


    randomSeed(analogRead(0));

    for(int indx = 0; indx  < 32; indx++)
    {
        addresses[indx] = random(2,255);
    }

    addresses[0] = 3;
    addresses[31] = 254;

    while(digitalRead(3) != HIGH){}

    if(digitalRead(3) == HIGH)
    {
        radio.openWritingPipe(defaultAddress);
        radio.stopListening();

        Serial.println("Sending");

        if(!radio.write(&addresses,32,0))
        {
            digitalWrite(ERROR_LED, HIGH);
            Serial.println("send failed");
            //while(true){}
        }
        else
        {
            Serial.println("Yay");
        }
    }

    if(digitalRead(2) == HIGH)
    {

        digitalWrite(ERROR_LED, LOW);
        radio.openReadingPipe(1,myAddress);
        radio.openWritingPipe(addresses[0]);
    }
}

void loop()
{

            numOfSpacers =0;
            indx = 0;
            command = "";
           
            //delay(300);

            radio.startListening();
            Serial.println("waiting..");

           //while(!radio.available()){}
            if(radio.available())
            {
                radio.read(&packedReply,32);
             
                Serial.println("Received message: ");
                for(int i =0; i < 32; i++)
                {
                    Serial.print(packedReply[i]);
                }
                
                //Get the index of the spacer before the command
                while(numOfSpacers != 1 && indx < 31)
                {
                    if(packedReply[indx] == '-')
                    {
                        numOfSpacers++;
                    }
                    else
                    {
                        indx++;
                    }
                }

                indx++;
                //Store the command
                startIndx = indx;
                for(byte indx2 = startIndx; packedReply[indx2] != '-';
                        indx2++)
                {
                    command += packedReply[indx2];
                }
               
                Serial.println(command);

                //Verify the command
                if(command == "P")
                {
                    Serial.println("sending back");
                    radio.stopListening();
                    radio.flush_tx();
                    
                    if(!radio.write(&testing,32))
                    {
                        Serial.println("SendBack Fail");
                    }
                    radio.txStandBy();
                }
                else if(command == "PD")
                {
                    while(true)
                    {
                        digitalWrite(STATUS_LED, HIGH);
                        delay(500);
                        digitalWrite(STATUS_LED, LOW);
                        delay(700);
                    }
                } 
            }
}
