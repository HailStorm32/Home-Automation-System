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
#include <queue.h>

#define DHTPIN 3     // Define what digital pin the temp sensor is connected to
#define DHTTYPE DHT11  //Define the type of temp sensor we are useing

RF24 radio(9, 10); //Set what pins the radio are in
DHT dht(DHTPIN, DHTTYPE); // Initialize temp sensor

const byte DEFAULT_ADDR = 2;//Address Min:2 Max:255
const byte MAX_NUM_OF_ADDRESSES = 32;
const byte STATUS_LED = A2;
const byte ERROR_LED = A3;
const byte SET_BTN = 2;
const byte EEPROM_ADDR = 0x50; //1010000
const byte MAX_ADDR_SENDING_CAP = 8;

struct node
{
    byte adjNodes[MAX_NUM_OF_ADDRESSES];
    byte parentNode;
    byte distanceFromStart;
};

byte addresses[MAX_NUM_OF_ADDRESSES] = {};//hub's address allways at [0] and server's at [31]
byte myAddress = 0;
byte serverAddr = 0;
node addrGraph[MAX_NUM_OF_ADDRESSES];
//byte addrGraph[MAX_NUM_OF_ADDRESSES][MAX_NUM_OF_ADDRESSES];//32x31
//byte nodeParents[MAX_NUM_OF_ADDRESSES];

bool hasGraph = false;

#include "include/functions.h"

//LOCAL VARIABLS
bool hasDoneSetup = false;
//char testing[20] = {};
char packedMessage[32];
String command;
byte fromAddress;
byte toAddress;
float temperature = 0;
int motionCount = 0;

void setup()
{
    Serial.begin(9600);//For testing
    pinMode(SET_BTN, INPUT);
    pinMode(STATUS_LED, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);
    
    //Startup radio and temperature sensors
    radio.begin();
	delay(200);
	dht.begin();
    

    radio.setRetries(15,15);
    radio.setChannel(124);
    //radio.setPALevel(RF24_PA_MIN);

    //Zero out addresses
    for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++)
    {
        addresses[indx] = 0;
    }

    //Zero out address graph
    for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++) //32
    {
        for(byte indx2 = 0; indx2 < MAX_NUM_OF_ADDRESSES; indx2++) //31
        {
            addrGraph[indx].adjNodes[indx2] = 0;
        }
        addrGraph[indx].parentNode = 0;
        addrGraph[indx].distanceFromStart = 255;
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

    //TODO:Get haveGraph status from eeprom

    //check storage to see it we have already done setup
    Serial.println("checking..");
    /*if(eepromReadSingle(0) == 1)
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
        for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++)
        {
            addresses[indx] = eepromReadSingle(indx+1);
        }

        myAddress = addresses[0];
        serverAddr = addresses[MAX_NUM_OF_ADDRESSES-1];

        //TODO: Get graph out of eeprom if we have one

        //For debug
        for(int i =0; i < MAX_NUM_OF_ADDRESSES; i++)
        {
            Serial.println(addresses[i]);
        }


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

        radio.read(&addresses, MAX_NUM_OF_ADDRESSES);

        //store the array we received into storage
        Serial.println("entering loop");
        for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++)
        {
            eepromWriteSingle(addresses[indx], indx+1);
        }

        myAddress = addresses[0];
        serverAddr = addresses[MAX_NUM_OF_ADDRESSES-1];

        Serial.println("received:");

        //For debug
        for(int i =0; i < MAX_NUM_OF_ADDRESSES; i++)
        {
            Serial.println(addresses[i]);
        }
        
        //Start placement mode
        placementMode(addresses, &radio); 
        Serial.println("Exit");     

        //store that we have finished setup in storage
        eepromWriteSingle(1, 0);
        hasDoneSetup = true;
    }*/

    radio.startListening();
    radio.openReadingPipe(1, myAddress);
    radio.openWritingPipe(serverAddr);
}

void loop()
{
    ////////////////DEBUG///////////////////////
    
    for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++)
    {
        addresses[indx] = 0;
    }


    addresses[0] = 20;
    addresses[1] = 21;
    addresses[2] = 22;
    addresses[3] = 23;
    addresses[4] = 24;
    addresses[9] = 25;


/////////////----A----//////////////
    addrGraph[0].adjNodes[0] = 23;//D
    addrGraph[0].adjNodes[1] = 22;//C
    addrGraph[0].adjNodes[2] = 21;//B

/////////////----B----//////////////
    addrGraph[1].adjNodes[0] = 22;//C
    addrGraph[1].adjNodes[1] = 24;//E
    addrGraph[1].adjNodes[2] = 25;//F
    addrGraph[1].adjNodes[3] = 20;//A

/////////////----C----//////////////
    addrGraph[2].adjNodes[0] = 20;//A
    addrGraph[2].adjNodes[1] = 21;//B
    addrGraph[2].adjNodes[2] = 24;//E

/////////////----D----//////////////
    addrGraph[3].adjNodes[0] = 20;//A
    addrGraph[3].adjNodes[1] = 24;//E

/////////////----E----//////////////
    addrGraph[4].adjNodes[0] = 23;//D
    addrGraph[4].adjNodes[1] = 21;//B
    addrGraph[4].adjNodes[2] = 22;//C

/////////////----F----//////////////
    addrGraph[9].adjNodes[0] = 21;//B

    /*Serial.println(numberOfValidEdges(getAddrIndx(20)));
    Serial.println(numberOfValidEdges(getAddrIndx(21)));
    Serial.println(numberOfValidEdges(getAddrIndx(22))); 
    Serial.println(numberOfValidEdges(getAddrIndx(23)));
    Serial.println(numberOfValidEdges(getAddrIndx(24)));
    Serial.println(numberOfValidEdges(getAddrIndx(25)));

    while(true){}*/

    myAddress = 20;
    serverAddr = 25;
    byte target = 24;

    hasGraph = true;


    Serial.println("Begining Test:");

    Serial.print("Node with shortest path is: ");

    Serial.println(findNodeWithShortestPath(&target));

    Serial.println("Done!");

    ////////////////DEBUG///////////////////////

    while(!radio.available()) {}

    radio.read(&packedMessage, 32);

    unpackMessage(packedMessage, &fromAddress, &toAddress, &temperature, 
            &motionCount, &command);

    if(toAddress == myAddress)
    {
        //If the command is Address Update Begin
        if(command == "AUB")
        {
            int *totalNumOfSends = &motionCount;
            //float *numOfAddrs2Recieve = &temperature;
            //byte numOfAddrs = 0;
            byte messageIndx = 0;
            byte addrIndx = 1;
            String strHolder;

            //Clear out the addresses we have stored, but keep our address
            //and the servers address
            for(byte indx = 1; indx < MAX_NUM_OF_ADDRESSES - 1; indx++)
            {
                addresses[indx] = 0;
            }
            
            for(byte indx = 0; indx < *totalNumOfSends; indx++)
            {
                //Tell server we are ready
                radio.stopListening();

                //TODO: Send message to server telling it we are ready
                sendMessage(&radio, serverAddr, 0,0, "RDY");

                //Wait for the dump of addresses
                while(!radio.available()) {}

                radio.read(&packedMessage, 32);

                //unpack the addresses
                while(packedMessage[messageIndx] != '&')
                {
                    while(packedMessage[messageIndx] != '-' 
                            || packedMessage[messageIndx] != '&')
                    {
                        strHolder += packedMessage[messageIndx];
                        messageIndx++;
                    }
                    
                    //Save the address into the array of address
                    addresses[addrIndx] = atoi(strHolder.c_str());

                    messageIndx++;
                    addrIndx++;
                    strHolder = "";
                }
            }

            //TODO: Check and see that we got the number of addresses the server
            //said we would get. If we didnt, send the server an error
        }
    }
    else
    {
        //TODO: Forward the message
    }
    
}
