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
#include <DHT.h>

#define DHTPIN 3     // Define what digital pin the temp sensor is connected to
#define DHTTYPE DHT11  //Define the type of temp sensor we are useing

RF24 radio(9, 10); //Set what pins the radio are in
DHT dht(DHTPIN, DHTTYPE); // Initialize temp sensor

const byte DEFAULT_ADDR = 2;//Address Min:2 Max:255
const byte NUM_OF_ADDRESSES = 32;
const byte STATUS_LED = A2;
const byte ERROR_LED = A3;
const byte SET_BTN = 2;
const byte EEPROM_ADDR = 0x50; //1010000
byte addresses[NUM_OF_ADDRESSES] = {};//hub's address allways at [0] and server's at [31]
byte myAddress = 0;
byte serverAddr = 0;
bool hasDoneSetup = false;
char testing[20] = {};

#include "include/functions.h"

void setup()
{
    Serial.begin(9600);//For testing
    pinMode(SET_BTN, INPUT);
//    pinMode(STATUS_LED, OUTPUT);
  //  pinMode(ERROR_LED, OUTPUT);
    
    //Startup radio and temperature sensors
    radio.begin();
	delay(200);
	dht.begin();
    

    radio.setRetries(15,15);
    radio.setChannel(124);
    //radio.setPALevel(RF24_PA_MIN);

    for(int indx  = 0; indx < NUM_OF_ADDRESSES; indx++)
    {
        addresses[indx] = 0;
    }
    
    myAddress = DEFAULT_ADDR;

    //Resest the eemprom
    if(digitalRead(SET_BTN) == LOW)
    {
        for(int indx = 0; indx < 2001; indx++)
        {
            digitalWrite(ERROR_LED, HIGH);
            eepromWriteSingle(0, indx);
            Serial.print("Erasing cell: ");
            Serial.println(indx);
            digitalWrite(ERROR_LED, LOW);
        }
    }

    //check storage to see it we have already done setup
    Serial.println("checking..");
    if(eepromReadSingle(0) == 1)
    {
        digitalWrite(STATUS_LED, HIGH);
        delay(300);
        digitalWrite(STATUS_LED, LOW);
        delay(300);
        digitalWrite(STATUS_LED, HIGH);
        delay(300);
        digitalWrite(STATUS_LED, LOW);
        
        Serial.println("have been in setup");
        hasDoneSetup = true;
        
        //get myAddress and other hub addresses from storage
        for(byte indx = 0; indx < NUM_OF_ADDRESSES; indx++)
        {
            addresses[indx] = eepromReadSingle(indx+1);
        }

        myAddress = addresses[0];
        serverAddr = addresses[31];

        //For debug
        for(int i =0; i < NUM_OF_ADDRESSES; i++)
        {
            Serial.println(addresses[i]);
        }

        /*String message;
        for(byte indx = 0; indx < 32; indx++)
        {
            message[indx] = '%';
        }

//        message = packMessage("SDH",134, 53,754.43,1332683);

        Serial.println("Message: ");
        Serial.print(message);*/

    }
    else
    {
        //wait for user confirmation
        while(digitalRead(SET_BTN) != LOW)
        {
            digitalWrite(STATUS_LED, HIGH);
            digitalWrite(ERROR_LED, HIGH);
            delay(300);
            digitalWrite(STATUS_LED, LOW);
            digitalWrite(ERROR_LED, LOW);
            delay(300);
            digitalWrite(STATUS_LED, HIGH);
            digitalWrite(ERROR_LED, HIGH);
            delay(300);
            digitalWrite(STATUS_LED, LOW);
            digitalWrite(ERROR_LED, LOW);
        }

        Serial.println("havnt been in setup");
        radio.openReadingPipe(1,myAddress);
        radio.startListening();

        digitalWrite(STATUS_LED, HIGH);

        //Wait to get the array of addresses from the server
        while(!radio.available())
        {}
        
        digitalWrite(STATUS_LED, LOW);

        radio.read(&addresses, NUM_OF_ADDRESSES);

        //store the array we received into storage
        Serial.println("entering loop");
        for(byte indx = 0; indx < NUM_OF_ADDRESSES; indx++)
        {
            eepromWriteSingle(addresses[indx], indx+1);
        }

        myAddress = addresses[0];
        serverAddr = addresses[31];

        Serial.println("received:");

        //For debug
        for(int i =0; i < NUM_OF_ADDRESSES; i++)
        {
            Serial.println(addresses[i]);
        }
        
        //Start placement mode
        placementMode(addresses, &radio); 
        Serial.println("Exit");     

        //store that we have finished setup in storage
        eepromWriteSingle(1, 0);
        hasDoneSetup = true;
    }

}

void loop()
{
    
}
