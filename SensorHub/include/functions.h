#include <Wire.h>
//#include <queue.h>

String packMessage(String command, byte targetAddr, byte senderAddr,
        float tempData, int motionData);
bool unpackMessage(char *packedMessage, byte *fromAddress, byte *toAddress, 
        float *temperature, int *motionCount, String *command);
void placementMode(byte *addresses, RF24 *radioP);
void eepromWriteSingle(byte data, int memAddr);
byte eepromReadSingle(int memAddr);
bool sendMessage(RF24 *radioP, byte receivingAddr, int motionCnt, 
        float temperature, String command);
byte getAddrIndx(byte *address);
byte findNodeWithShortestPath(byte *targetNode);
//byte numberOfValidEdges(byte nodeIndx);




//=======================================================================
//Description: Begins placement mode. Will continuously ping server and 
//  and report via LED whether or not its within range of server either
//  directly or through nearby hubs.
//
//Arguments:
//  (OUT) addresses -- byte array of 32 address
//  (IN)  radioP -- pointer to radio instance
//
//Return:
//  void
//=======================================================================
void placementMode(byte *addresses, RF24 *radioP)
{
    byte myAddress = addresses[0];
    byte serverAddr = addresses[MAX_NUM_OF_ADDRESSES-1];
    byte indx = 0;
    byte startIndx;
    byte numOfSpacers = 0;
    char packedReply[32] = {};
    String command;
    String message;
    bool messageSent = false;
    bool timeOut = false;
    bool receivedReply = false;
    

    radioP->stopListening();
    radioP->flush_tx();
    radioP->openWritingPipe(serverAddr);
    radioP->openReadingPipe(1, myAddress);

    message = packMessage("P", serverAddr, myAddress, 0, 0); 
   
    while(digitalRead(SET_BTN) == HIGH || receivedReply != true)
    {
        indx = 1;
        numOfSpacers = 0;
        command = "";
        messageSent = false;
        timeOut = false;
        radioP->stopListening();
        radioP->flush_tx();
    
        if(!radioP->write(message.c_str(), 32))
        {
            radioP->txStandBy();
            //Try and forward to other addresses
            while(addresses[indx] != 0 && addresses[indx] != serverAddr)
            {
                radioP->openWritingPipe(addresses[indx]);

                if(radioP->write(message.c_str(), 32))
                {
                    radioP->txStandBy();
                    messageSent = true;
                    break;
                }
                radioP->txStandBy();
                indx++;
            }
            radioP->openWritingPipe(serverAddr);
            indx = 0;
        }
        else
        {
            Serial.println("First try");
            messageSent =  true;
        }

        //Wait for a reply only if we were able to send the message
        if(messageSent)
        {
            receivedReply = false;
            long unsigned int endTime = 0; 
            radioP->startListening();

            endTime = millis() + 1000;

            Serial.println("Waiting..");
            while(!radioP->available())
            {
                if(millis() >= endTime)
                {
                    timeOut = true;
                    break;
                }
            }

            if(radioP->available() && timeOut == false)
            {
                radioP->read(&packedReply,32);
                Serial.println("Received reply"); 
                //Get the index of the spacer before the command
                while(numOfSpacers != 1 && indx < 32)
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
                if(command == "PR")
                {
                    Serial.println("Verified");
                    digitalWrite(ERROR_LED, LOW);
                    digitalWrite(STATUS_LED, HIGH);
                    receivedReply = true;
                }
            }
            else
            {
                radioP->flush_rx();
            }

            if(receivedReply == false)
            {
                Serial.println("Did not receive reply");
                digitalWrite(ERROR_LED, HIGH);
                digitalWrite(STATUS_LED, LOW);
            }
        }
        else
        {
            Serial.println("Could not send");
            digitalWrite(ERROR_LED, HIGH);
            digitalWrite(STATUS_LED, LOW);
            receivedReply = false;
        }
    }
    
    radioP->openWritingPipe(serverAddr);
    radioP->stopListening();
    radioP->flush_tx();

    message = packMessage("PD", serverAddr, myAddress, 0, 0); 

    radioP->write(message.c_str(), 32);
    radioP->txStandBy();
    
    digitalWrite(ERROR_LED, LOW);
    digitalWrite(STATUS_LED, LOW);
    delay(300);

    digitalWrite(STATUS_LED, HIGH);
    delay(500);
    digitalWrite(STATUS_LED, LOW);
    delay(700);
    digitalWrite(STATUS_LED, HIGH);
    delay(500);
    digitalWrite(STATUS_LED, LOW);
    delay(700);
    digitalWrite(STATUS_LED, HIGH);
    delay(500);
    digitalWrite(STATUS_LED, LOW);
}




//=========================================================================
//Description: Packs all the data needed to be sent into a 32 char array
//
//Arguments:
//  (IN) targetAddr -- single byte address of where this packet is going
//  (IN) senderAddr -- single byte address of sender address
//  (IN) tempData -- data from the temperature sensor
//  (IN) motionData -- data from the motion sensor
//  (IN) command -- 1-3 char command
//
//Return:
//  packedMessage -- string containing the packed message
//==========================================================================
String packMessage(String command, byte targetAddr, byte senderAddr,
        float tempData = 0, int motionData = 0)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, &]
	
    255-S-70.3-12-245&
    */

    char packedMsg [32] = {};
    String numberHolder;
    String packedStr;
    byte indxCounter = 0;
    byte indxStart = 0;

    for(byte indx = 0; indx < 32; indx++)
    {
        packedMsg[indx] = '0';
    }

    //Add targetAddr to message array
    numberHolder = static_cast<String>(targetAddr);     
    for(byte indx = 0; indx < numberHolder.length(); indx++)
    {
        packedMsg[indx] = numberHolder[indx];
        indxCounter++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;

    //Add command to message array
    indxStart = indxCounter; 
    if(command.length() >= 3)
    {
        for(byte indx = 0; indx < 3; indx++)
        {
            packedMsg[indx + indxStart] = command[indx];
            indxCounter++;
        }
    }
    else
    {
        for(byte indx = 0; indx < command.length(); indx++)
        {
            packedMsg[indx + indxStart] = command[indx];
            indxCounter++;
        }
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;

    //Add tempData to message array
    numberHolder = static_cast<String>(tempData);
   
    indxStart = indxCounter; 
    //Limit the length of the number to 7 chars (includes decimal)
    if(numberHolder.length() >= 7)
    {
        for(byte indx = 0; indx < 7; indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }
    else
    {
        for(byte indx = 0; indx < numberHolder.length(); indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    
    //Add motion to message array
    numberHolder = static_cast<String>(motionData);
    
    indxStart = indxCounter; 
    //Limit the length of the number to 7 chars
    if(numberHolder.length() >= 7)
    {
        for(byte indx = 0; indx < 7; indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }
    else
    {
        for(byte indx = 0; indx < numberHolder.length(); indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }
     
    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    
    indxStart = indxCounter;
    //Add senderAddr to message array
    numberHolder = static_cast<String>(senderAddr);     
    for(byte indx = 0; indx < numberHolder.length(); indx++)
    {
        packedMsg[indx + indxStart] = numberHolder[indx];
        indxCounter++;
    }

    //Add end char
    packedMsg[indxCounter] = '&';

    packedStr = packedMsg;

    return packedStr;
}


//=====================================================================
//Description: unpacks the command, sender address and reciver address
//  from the given packed message
//
//Arguments:
//  (IN) packedMessage -- the packed message
//  (OUT) fromAddress -- returns the senders address
//  (OUT) toAddress -- returns the address that the message was addressed to
//  (OUT) command -- returns the command that was in the message
//
//Return:
//  true -- was able to unpack the command
//  false -- unpacking failed

bool unpackMessage(char *packedMessage, byte *fromAddress, byte *toAddress, 
        float *temperature, int *motionCount, String *command)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, &]
	
    255-S-70.3-12-245&
    */

    byte indx = 0;
    String stringHolder;
    
    //TODO: Add a check to make sure the message was formated right

    //Get the toAddress
    while(packedMessage[indx] != '-')
    {
        stringHolder += packedMessage[indx];
        indx++;
    }

    *toAddress = atoi(stringHolder.c_str());

    indx++;
    stringHolder = "";

    //Get the command
    while(packedMessage[indx] != '-')
    {
        stringHolder += packedMessage[indx];
        indx++;
    }

    *command = stringHolder;
    indx++;
    stringHolder = "";

    //Get temperature data
    while(packedMessage[indx] != '-')
    {
        stringHolder += packedMessage[indx];
        indx++;
    }
    *temperature = atof(stringHolder.c_str());

    indx++;
    stringHolder = "";

    //Get motion count data
    while(packedMessage[indx] != '-')
    {
        stringHolder += packedMessage[indx];
        indx++;
    }
    *motionCount = atoi(stringHolder.c_str());

    indx++;
    stringHolder = "";

    //Get the fromAddress
    while(packedMessage[indx] != '&')
    {
        stringHolder += packedMessage[indx];
        indx++;
    }

    *fromAddress = atoi(stringHolder.c_str());

    return true;
}

//=====================================================================
//Description: writes a byte of data to the given address of the 
//  external eeprom
//
//Arguments:
//  (IN) data -- byte of data to write
//  (IN) memAddr -- memory address of cell to write to
//
//Return:
//  void
//======================================================================
void eepromWriteSingle(byte data, int memAddr)
{
    Wire.begin();
   
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write(memAddr);
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
}


//======================================================================
//Description: reads a single byte from the given cell address of the
//  external eeprom
//
//Arguments:
//  (IN) memAddr -- memory address of cell to read from
//
//Return:
//  byte -- the byte read from the given cell
//=======================================================================
byte eepromReadSingle(int memAddr)
{
    Wire.begin();
    
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write(memAddr);
    Wire.endTransmission();
   
    Wire.requestFrom(EEPROM_ADDR,(byte)1);
    
    return Wire.read();
}


//========================================================================
//Description: sends a message to the given address using the address
//  graph (if it has been set up) to send out messages
//
//Arguments:
//  (IN) receivingAddr -- address to send message to
//  (IN) motionCnt -- number of motion triggers
//  (IN) temperature -- room temperature
//  (IN) command -- command
//
//Return:
//  true -- was able to send to a hub
//  false -- wasnt able to send to any hub
//=========================================================================
bool sendMessage(RF24 *radioP, byte receivingAddr, int motionCnt, 
        float temperature, String command)
{
    String packedMessage;
    byte addrIndx = 1;
    bool messageSent = false;
    //byte hubsInRange

    radioP->stopListening();
    radioP->openWritingPipe(receivingAddr);

    packedMessage = packMessage(command, receivingAddr, myAddress, temperature,
            motionCnt);

    if(radioP->write(packedMessage.c_str(), 32))
    {
        messageSent = true;
    }
    else
    {
        if(hasGraph == true)
        {
            //TODO: Send message using graph
            
            //See if one of our in range hubs can reach the address
        }
        else
        {
            while(addresses[addrIndx] != 0 || addrIndx != 
                    MAX_NUM_OF_ADDRESSES-1)
            {
                radioP->openWritingPipe(addresses[addrIndx]);
                if(radioP->write(packedMessage.c_str(), 32))
                {
                    messageSent = true;
                    break;
                }
                addrIndx++;
            }
        }
    }
    
    if(messageSent == true)
    {
        radioP->openWritingPipe(serverAddr);
        radioP->startListening();
        return true;
    }
    else
    {
        radioP->openWritingPipe(serverAddr);
        radioP->startListening();
        return false;
    }
}


//=========================================================================
//Description: returns what index the given address is stored in the
//  address array
//
//Arguments:
//  (IN) address -- address to get the index for
//
//Returns:
//  byte -- the index for the given address
//=========================================================================
byte getAddrIndx(byte *address)
{
    byte indx = 0;

    while(indx != MAX_NUM_OF_ADDRESSES && addresses[indx] != *address)
    {
        indx++;
    }

    if(indx >= MAX_NUM_OF_ADDRESSES)
    {
        indx = 255;
        Serial.print("###");
        while(true){}
        return indx;
    }

    /*Serial.print("I:");
    Serial.println(indx);*/
   
    return indx;
}


//=========================================================================
//
//=========================================================================
/*byte numberOfValidEdges(byte nodeIndx)
{
    byte numOfValidEdges = 0;

    for(byte edgeIndx = 0; edgeIndx < MAX_NUM_OF_ADDRESSES; edgeIndx++)
    {
        if(addrGraph[nodeIndx][edgeIndx] != 0)
        {
            numOfValidEdges++;
        }
    }
    
    return numOfValidEdges;
}*/


//=========================================================================
//
//=========================================================================
byte findNodeWithShortestPath(byte *targetNode)
{
    bool haveVisited[MAX_NUM_OF_ADDRESSES];
    //byte previusNode = 0;
    byte currentNode = 0;
    byte indx;
    byte adjNode;
    Queue graphQueue;

    for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++)
    {
        haveVisited[indx] = false;
    }

    currentNode = myAddress; 
    graphQueue.push(currentNode);
    haveVisited[getAddrIndx(&currentNode)] = true;

    while(!graphQueue.isEmpty())
    {
        graphQueue.pop();

        //Queue all adjacent nodes
        while(addrGraph[getAddrIndx(&currentNode)].adjNodes[indx] != 0)
        {
            adjNode = addrGraph[getAddrIndx(&currentNode)].adjNodes[indx];

            if(haveVisited[getAddrIndx(&adjNode)] == false)
            {
                //Add to queue
                graphQueue.push(adjNode);

                //Mark as visited
                haveVisited[getAddrIndx(&adjNode)] = true;

                //Add its parent node
                addrGraph[getAddrIndx(&adjNode)].parentNode = currentNode;

                //Get its distance to the starting node
                addrGraph[getAddrIndx(&adjNode)].distanceFromStart = 
                    addrGraph[getAddrIndx(&currentNode)].distanceFromStart + 1;
            }
            indx++;
        }

        currentNode = graphQueue.front();
        indx = 0;
    }
    
    //Work backward from the target node and piece together parent nodes
    indx = 0;

    currentNode = *targetNode;

    while(addrGraph[getAddrIndx(&currentNode)].parentNode != myAddress)
    {
        currentNode = addrGraph[getAddrIndx(&currentNode)].parentNode;
    }
    return currentNode;
}
