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

#define DHTPIN 6     // Define what digital pin the temp sensor is connected to
#define DHTTYPE DHT11  //Define the type of temp sensor we are useing

RF24 radio(9, 10); //Set what pins the radio are in
DHT dht(DHTPIN, DHTTYPE); // Initialize temp sensor

const byte DEFAULT_ADDR = 2;//Address Min:2 Max:255
const byte NUM_OF_ADDRESSES = 32;
const byte STATUS_LED = 8;
const byte ERROR_LED = 7;
const byte SET_BTN = 4;
byte addresses[NUM_OF_ADDRESSES] = {};//hub's address allways at [0] and server's at [31]
byte myAddress = 0;
byte serverAddr = 0;
bool hasDoneSetup = false;
char testing[20] = {};

#include "include/functions.h"

void setup()
{
    Serial.begin(9600);//For testing
    pinMode(2, INPUT);//For testing
    pinMode(STATUS_LED, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);
    
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

    //TODO check storage to see it we have already done settup
    //currently using an external btn as current boards dont have storage
    if(digitalRead(2) == HIGH)
    {
        Serial.println("have been in setup");
        hasDoneSetup = true;
        
        //TODO get myAddress and address array from storage
        //currently using pre set values as current board dont have storage
        addresses[0] = 2;
        addresses[1] = 3;

        myAddress = addresses[0];

        String message;
        for(byte indx = 0; indx < 32; indx++)
        {
            message[indx] = '%';
        }

//        message = packMessage("SDH",134, 53,754.43,1332683);

        Serial.println("Message: ");
        Serial.print(message);

    }
    else
    {
        Serial.println("havnt been in setup");
        radio.openReadingPipe(1,myAddress);
        radio.startListening();

        digitalWrite(STATUS_LED, HIGH);

        while(!radio.available())
        {}
        
        digitalWrite(STATUS_LED, LOW);

        radio.read(&addresses, NUM_OF_ADDRESSES);
        //TODO store the array we received into storage

        myAddress = addresses[0];
        serverAddr = addresses[31];

        Serial.println("received:");

        for(int i =0; i < NUM_OF_ADDRESSES; i++)
        {
            Serial.println(addresses[i]);
        }
        
        //Start placement mode
        placementMode(addresses, &radio); 
        Serial.println("Exit");     
    }

}

void loop()
{
    
}
