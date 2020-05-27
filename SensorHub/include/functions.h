#include <i2c_t3.h>
//#include <queue.h>

bool packMessage(char *command, char *packedMessage, byte targetAddr, 
        byte senderAddr, float tempData, short motionData);
bool unpackMessage(char *packedMessage, byte *fromAddress, byte *toAddress, 
        float *temperature, short *motionCount, char *command, bool duplicateMsg);
void placementMode(RF24 *radioP, byte myAddress, byte serverAddr);
void eepromWriteSingle(byte data, short memAddr);
byte eepromReadSingle(short memAddr);
bool sendMessage(RF24 *radioP, byte receivingAddr, short motionCnt, 
        float const * const temperature, char *command);
byte getAddrIndx(byte address);
byte findNodeWithShortestPath(byte targetNode, byte startingNode);
void ftoa(float fVal, char *strVal);
bool eepromRetrieveGraph();
bool eepromStoreGraph();
byte eepromRetrieveMsgId(const byte hub_addr);
void eepromStoreMsgId(const byte HUB_ADDR, const byte ID);
byte generateMsgId(const byte HUB_ADDR);



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
void placementMode(RF24 *radioP, byte myAddress, byte serverAddr)
{
    byte indx = 0;
    byte startIndx;
    byte numOfSpacers = 0;
    char packedReply[32] = {};
    char command[COMMAND_SIZE];
    char packedMsg[32];
    bool messageSent = false;
    bool timeOut = false;
    bool receivedReply = false;
    
    Serial.print(F("My: "));
    Serial.println(myAddress);
    radioP->stopListening();
    radioP->flush_tx();
    radioP->openWritingPipe(serverAddr);
    radioP->openReadingPipe(1, myAddress);


    //packMessage("P__", packedMsg, *serverAddr, *myAddress, 0, 0); 
        /*for(byte indx = 0; indx < 32; indx++)
        {
            Serial.print(packedMsg[indx]);
        }
        Serial.println(F(" "));*/
   
    while(digitalRead(SET_BTN) == HIGH || receivedReply != true)
    {
        packMessage("P__", packedMsg, serverAddr, myAddress, 0, 0); 

        indx = 1;
        numOfSpacers = 0;
        command[COMMAND_SIZE] = '\0';
        messageSent = false;
        timeOut = false;
        for(byte indx = 0; indx < COMMAND_SIZE - 1; indx++)
        {
            command[indx] = 'X';
        }
        command[COMMAND_SIZE] = '\0';
        
        radioP->stopListening();
        radioP->flush_tx();
    
        for(byte indx = 0; indx < 32; indx++)
        {
            Serial.print(packedMsg[indx]);
        }
        Serial.println(F(" "));

        if(!radioP->write(packedMsg, 32))
        {
            radioP->txStandBy();
            //Try and forward to other addresses
            while(addresses[indx] != 0 && addresses[indx] != serverAddr && 
                    indx < MAX_NUM_OF_ADDRESSES)
            {
                radioP->openWritingPipe(addresses[indx]);

                if(radioP->write(packedMsg, 32))
                {
                    //radioP->txStandBy();
                    messageSent = true;
                    //break;
                }
                radioP->txStandBy();
                indx++;
            }
            radioP->openWritingPipe(serverAddr);
            indx = 0;
        }
        else
        {
            Serial.println(F("First try"));
            messageSent =  true;
        }

        //Wait for a reply only if we were able to send the message
        if(messageSent)
        {
            receivedReply = false;
            long unsigned int endTime = 0; 
            radioP->startListening();

            endTime = millis() + 1000;

            Serial.println(F("Waiting.."));
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
                radioP->read(packedReply,32);
                
                //TODO: check to see if we recived this message already

                Serial.println(F("Received reply")); 
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
                    command[indx2 - startIndx] = packedReply[indx2];
                }
                Serial.println(command);        
                //Verify the command
                if(strcmp(command, "PR_") == 0)
                {
                    Serial.println(F("Verified"));
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
                Serial.println(F("Did not receive reply"));
                digitalWrite(ERROR_LED, HIGH);
                digitalWrite(STATUS_LED, LOW);
            }
        }
        else
        {
            Serial.println(F("Could not send"));
            digitalWrite(ERROR_LED, HIGH);
            digitalWrite(STATUS_LED, LOW);
            receivedReply = false;
        }
    }
    
    radioP->openWritingPipe(serverAddr);
    radioP->stopListening();
    radioP->flush_tx();
    indx = 0;

    packMessage("PD_", packedMsg, serverAddr, myAddress, 0, 0); 

    if(!radioP->write(packedMsg, 32))
    {
        radioP->txStandBy();
        //Try and forward to other addresses
        while(addresses[indx] != 0 && addresses[indx] != serverAddr && 
                indx < MAX_NUM_OF_ADDRESSES)
        {
            radioP->openWritingPipe(addresses[indx]);

            if(radioP->write(packedMsg, 32))
            {
                //radioP->txStandBy();
                messageSent = true;
                //break;
            }
            radioP->txStandBy();
            indx++;
        }
        radioP->openWritingPipe(serverAddr);
        indx = 0;
    }
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
//  (IN) packedMessage -- char array containing the packed message
//
//Return:
//  true -- was packed
//==========================================================================
bool packMessage(char *command, char *packedMessage, byte targetAddr, 
        byte senderAddr, float tempData = 0.0, short motionData = 0)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> messageID -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, -, 1, 4, 0,&]
	
    255-S-70.3-12-245-140&
    */

    const byte HOLDER_SIZE = 6;
    char packedMsg [32] = {};
    char numberHolder[HOLDER_SIZE] = {}; //{'X','X','X','X','X','\0'};
    //byte numHolderIndx = 0;
    //String packedStr;
    byte indxCounter = 0; 
    byte indxStart = 0;
    byte genIndx = 0;

    for(byte indx = 0; indx < 32; indx++)
    {
        packedMsg[indx] = '0';
        packedMessage[indx] = '0';
    }
    for(byte indx = 0; indx < HOLDER_SIZE - 1; indx++)
    {
        numberHolder[indx] = 'X';
    }
    numberHolder[HOLDER_SIZE] = '\0';

    //Add targetAddr to message array
    itoa(targetAddr, numberHolder, 10);
    for(byte indx = 0; indx < HOLDER_SIZE; indx++)
    {
        Serial.print(numberHolder[indx]);
    }
    Serial.println(F(" "));

    while(genIndx < HOLDER_SIZE - 1 && numberHolder[genIndx] != 'X'
            && numberHolder[genIndx] != '\0')
    {
        packedMsg[genIndx] = numberHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;

    //Add command to message array
    indxStart = indxCounter; 
    while(genIndx < COMMAND_SIZE && command[genIndx] != '\0')
    {
        packedMsg[genIndx + indxStart] = command[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;
    for(byte indx = 0; indx < HOLDER_SIZE - 1; indx++)
    {
        numberHolder[indx] = 'X';
    }
    for(byte indx = 0; indx < 5; indx++)
    {
        numberHolder[indx] = '0';
    }

    //Add tempData to message array
    ftoa(tempData, numberHolder); 
    indxStart = indxCounter; 
    while(genIndx < HOLDER_SIZE - 1 && numberHolder[genIndx] != 'X'
            && numberHolder[genIndx] != '\0')
    {
        packedMsg[genIndx + indxStart] = numberHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;
    for(byte indx = 0; indx < HOLDER_SIZE - 1; indx++)
    {
        numberHolder[indx] = 'X';
    }
    
    //Add motion to message array
    itoa(motionData, numberHolder, 10);
    
    indxStart = indxCounter; 
    while(genIndx < HOLDER_SIZE - 1 && numberHolder[genIndx] != 'X'
            && numberHolder[genIndx] != '\0')
    {
        packedMsg[genIndx + indxStart] = numberHolder[genIndx];
        indxCounter++;
        genIndx++;
    }
     
    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;
    for(byte indx = 0; indx < HOLDER_SIZE - 1; indx++)
    {
        numberHolder[indx] = 'X';
    }

    //Add senderAddr to message array
    itoa(senderAddr, numberHolder, 10);

    indxStart = indxCounter;
    while(genIndx < HOLDER_SIZE - 1 && numberHolder[genIndx] != 'X'
            && numberHolder[genIndx] != '\0')
    {
        packedMsg[genIndx + indxStart] = numberHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;
    for(byte indx = 0; indx < HOLDER_SIZE - 1; indx++)
    {
        numberHolder[indx] = 'X';
    }

    itoa(generateMsgId(senderAddr), numberHolder, 10);
    
    indxStart = indxCounter; 
    while(genIndx < HOLDER_SIZE - 1 && numberHolder[genIndx] != 'X'
            && numberHolder[genIndx] != '\0')
    {
        packedMsg[genIndx + indxStart] = numberHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add end char
    packedMsg[indxCounter] = '&';

    
    for(byte indx = 0; indx < 32; indx++)
    {
        packedMessage[indx] = packedMsg[indx];
    }

    return true;
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
//  (OUT) duplicateMsg -- boolean value, true if message has been recieved before
//
//Return:
//  true -- was able to unpack the command
//  false -- unpacking failed
//=====================================================================
bool unpackMessage(char *packedMessage, byte *fromAddress, byte *toAddress, 
        float *temperature, short *motionCount, char *command, bool duplicateMsg)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> messageID -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, -, 1, 4, 0,&]
	
    255-S-70.3-12-245-140&
    */

    byte indx = 0;
    byte stringHolderIndx = 0;
    byte messageId = 0;
    char stringHolder[11];
    
    for(byte indx2 = 0; indx2 < 10; indx2++)
    {
        stringHolder[indx2] = 'X';
    }
    stringHolder[10] = '\0';

    //TODO: Add a check to make sure the message was formated right

    //Get the toAddress
    while(packedMessage[indx] != '-')
    {
        stringHolder[stringHolderIndx] = packedMessage[indx];
        indx++;
        stringHolderIndx++;
    }

    *toAddress = atoi(stringHolder);

    indx++;
    stringHolderIndx = 0;
    for(byte indx2 = 0; indx2 < 10; indx2++)
    {
        stringHolder[indx2] = 'X';
    }

    //Get the command
    while(packedMessage[indx] != '-')
    {
        command[stringHolderIndx] = packedMessage[indx];
        indx++;
        stringHolderIndx++;
    }

    indx++;
    stringHolderIndx = 0;
    for(byte indx2 = 0; indx2 < 10; indx2++)
    {
        stringHolder[indx2] = 'X';
    }

    //Get temperature data
    while(packedMessage[indx] != '-')
    {
        stringHolder[stringHolderIndx] = packedMessage[indx];
        indx++;
        stringHolderIndx++;
    }
   *temperature = atof(stringHolder);

    indx++;
    stringHolderIndx = 0;
    for(byte indx2 = 0; indx2 < 10; indx2++)
    {
        stringHolder[indx2] = 'X';
    }

    //Get motion count data
    while(packedMessage[indx] != '-')
    {
        stringHolder[stringHolderIndx] = packedMessage[indx];
        indx++;
        stringHolderIndx++;
    }
    *motionCount = atoi(stringHolder);

    indx++;
    stringHolderIndx = 0;
    for(byte indx2 = 0; indx2 < 10; indx2++)
    {
        stringHolder[indx2] = 'X';
    }

    //Get the fromAddress
    while(packedMessage[indx] != '-')
    {
        stringHolder[stringHolderIndx] = packedMessage[indx];
        indx++;
        stringHolderIndx++;
    }

    *fromAddress = atoi(stringHolder);

    indx++;
    stringHolderIndx = 0;
    for(byte indx2 = 0; indx2 < 10; indx2++)
    {
        stringHolder[indx2] = 'X';
    }

    //Get the message ID
    while(packedMessage[indx] != '&')
    {
        stringHolder[stringHolderIndx] = packedMessage[indx];
        indx++;
        stringHolderIndx++;
    }

    messageId = atoi(stringHolder);

    if(messageId == eepromRetrieveMsgId(*fromAddress))
    {
        duplicateMsg = true;
    }
    else
    {
        duplicateMsg = false;
        eepromStoreMsgId(*fromAddress, messageId);
    }

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
void eepromWriteSingle(byte data, short memAddr)
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
byte eepromReadSingle(short memAddr)
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
bool sendMessage(RF24 *radioP, byte receivingAddr, short motionCnt, 
        float const * const temperature, char *command)
{
    byte myAddress = addresses[0];
    byte serverAddr = addresses[MAX_NUM_OF_ADDRESSES - 1]; 
    char packedMessage[32];
    byte addrIndx = 1;
    bool messageSent = false;
    byte hub2SendTo;

    radioP->stopListening();
    radioP->openWritingPipe(receivingAddr);

    packMessage(command, packedMessage, receivingAddr, myAddress, *temperature,
           motionCnt);

    if(radioP->write(packedMessage, 32))
    {
        messageSent = true;
    }
    else
    {
        if(hasGraph == true)
        {
            byte indx = 0;
            hub2SendTo = findNodeWithShortestPath(receivingAddr, myAddress);
            
            if(!radioP->write(packedMessage, 32))
            {
                //try and see if there is a path from another adjacent hub
                while(indx < MAX_NUM_OF_ADDRESSES && 
                        addrGraph[getAddrIndx(myAddress)].adjNodes[indx] != 0)
                {
                    hub2SendTo = findNodeWithShortestPath(
                            addrGraph[getAddrIndx(myAddress)]
                            .adjNodes[indx], myAddress);

                    if(hub2SendTo != 0 && radioP->write(packedMessage, 32))
                    {
                        messageSent = true;
                        break;
                    }
                }
            }
            else
            {
                messageSent = true;
            }
        }
        else
        {
            while(addresses[addrIndx] != 0 || addrIndx != 
                    MAX_NUM_OF_ADDRESSES-1)
            {
                radioP->openWritingPipe(addresses[addrIndx]);
                if(radioP->write(packedMessage, 32))
                {
                    messageSent = true;
                    break;
                }
                addrIndx++;
            }
        }
    }
    radioP->openWritingPipe(serverAddr);
    radioP->startListening();
    
    if(messageSent == true)
    {
        return true;
    }
    else
    {
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
byte getAddrIndx(byte address)
{
    byte indx = 0;

    while(indx != MAX_NUM_OF_ADDRESSES && addresses[indx] != address)
    {
        indx++;
    }

    if(indx >= MAX_NUM_OF_ADDRESSES)
    {
        indx = 255;
        Serial.print(F("Error in 'getAddrIndx'"));
        while(true){}
        return indx;
    }

   
    return indx;
}


//=========================================================================
//Description: Traverses address graph using BFS. Then using the list of
//  parent nodes, goes backwards from the target node to find the node that
//  starts the shortest path
//
//Arguments:
//  (IN) targetNode -- contains the address of the target node
//  (IN) startingNode -- contains the address of the starting node
//
//Returns:
//  0 -- if failed
//  2-255 -- address of the node that starts the path
//=========================================================================
byte findNodeWithShortestPath(byte targetNode, byte startingNode)
{
    bool haveVisited[MAX_NUM_OF_ADDRESSES];
    const byte myAddress = addresses[0];
    byte currentNode = 0;
    byte indx;
    byte adjNode;
    Queue graphQueue;

    for(byte indx = 0; indx < MAX_NUM_OF_ADDRESSES; indx++)
    {
        haveVisited[indx] = false;
    }

    if(startingNode == 0)
    {
        currentNode = myAddress;
    }
    else
    {
        currentNode = startingNode; 
    }
    graphQueue.push(currentNode);
    haveVisited[getAddrIndx(currentNode)] = true;

    while(!graphQueue.isEmpty())
    {
        graphQueue.pop();

        //Queue all adjacent nodes
        while(addrGraph[getAddrIndx(currentNode)].adjNodes[indx] != 0)
        {
            adjNode = addrGraph[getAddrIndx(currentNode)].adjNodes[indx];

            if(haveVisited[getAddrIndx(adjNode)] == false)
            {
                //Add to queue
                graphQueue.push(adjNode);

                //Mark as visited
                haveVisited[getAddrIndx(adjNode)] = true;

                //Add its parent node
                addrGraph[getAddrIndx(adjNode)].parentNode = currentNode;

                //Get its distance to the starting node
                //addrGraph[getAddrIndx(&adjNode, addresses)].distanceFromStart = 
                   // addrGraph[getAddrIndx(&currentNode, addresses)].distanceFromStart + 1;
            }
            indx++;
        }

        currentNode = graphQueue.front();
        indx = 0;
    }
    
    //Work backward from the target node and piece together parent nodes
    indx = 0;

    currentNode = targetNode;

    while(addrGraph[getAddrIndx(currentNode)].parentNode != myAddress &&
            indx < MAX_NUM_OF_ADDRESSES)
    {
        currentNode = addrGraph[getAddrIndx(currentNode)].parentNode;
        indx++;
    }
    if(indx >= MAX_NUM_OF_ADDRESSES)
    {
        return 0;
    }
    else
    {
        return currentNode;
    }
}


//============================================================================
//Description: Takes a float value and converts to c-string
//  Credit to enhzflep from Stackoverflow
//  https://stackoverflow.com/a/23191607
//
//Arguments:
//  (IN) fVal -- float value
//  (OUT) strVal -- c-string to contain result
//
//Returns:
//  void
//============================================================================
void ftoa(float fVal, char *strVal)
{
    int dVal, dec, indx;
    char valHold[strlen(strVal)];

    fVal += 0.005;

    dVal = fVal;
    dec = (int)(fVal * 100) % 100;

    strcpy(valHold, "0");
    strcpy(strVal, "0");
    strVal[0] = (dec % 10) + '0';
    strVal[1] = (dec / 10) + '0';
    strVal[2] = '.';

    indx = 3;
    while (dVal > 0)
    {
        strVal[indx] = (dVal % 10) + '0';
        dVal /= 10;
        indx++;
    }
    
    strcpy(valHold, strVal);
    indx = (strlen(strVal) - 1);

    while(indx >= 0)
    {
        strVal[(strlen(strVal) - 1) - indx] = valHold[indx];
        indx--;
    }
}



//============================================================================
//Description: Stores the addrGraph into eeprom storage
//
//Arguments:
//  void
//Returns:
//  true -- was able to store graph
//  false --- was not able to store graph
//============================================================================
bool eepromStoreGraph()
{
    short memIndx = GRAPH_ARRAY_MEM_START;

    for(byte nodeIndx = 0; nodeIndx < MAX_NUM_OF_ADDRESSES; nodeIndx++)
    {
        if(memIndx > 2000)
        {
            return false;
        }
        
        for(byte addrIndx = 0; addrIndx < MAX_NUM_OF_ADDRESSES; addrIndx++)
        {
            if(memIndx > 2000)
            {
                return false;
            }
            eepromWriteSingle(addrGraph[nodeIndx].adjNodes[addrIndx], memIndx);
            memIndx++;
        }
        eepromWriteSingle(addrGraph[nodeIndx].parentNode, memIndx);
        memIndx++;
    }
    return true;
}


//============================================================================
//Description: Gets the addrGraph into eeprom storage
//
//Arguments:
//  void
//Returns:
//  true -- was able to get graph
//  false --- was not able to get graph
//============================================================================
bool eepromRetrieveGraph()
{
    short memIndx = GRAPH_ARRAY_MEM_START;

    for(byte nodeIndx = 0; nodeIndx < MAX_NUM_OF_ADDRESSES; nodeIndx++)
    {
        if(memIndx > 2000)
        {
            return false;
        }

        for(byte addrIndx = 0; addrIndx < MAX_NUM_OF_ADDRESSES; addrIndx++)
        {
            if(memIndx > 2000)
            {
                return false;
            }
            addrGraph[nodeIndx].adjNodes[addrIndx] = eepromReadSingle(memIndx);
            memIndx++;
        }
        addrGraph[nodeIndx].parentNode = eepromReadSingle(memIndx);
        memIndx++;
    }
    return true;
}



//============================================================================
//Description: Gets the last message ID used by the specified hub address 
//  from eeprom
//
//Arguments:
//  (IN) HUB_ADDR -- address of the hub to look up
//
//Return:
//  byte -- the last used ID by the given hub address
//============================================================================
byte eepromRetrieveMsgId(const byte HUB_ADDR)
{
    return eepromReadSingle(getAddrIndx(HUB_ADDR) 
            + GRAPH_ARRAY_MEM_START);
}


//============================================================================
//Description: Stores the message ID used by the specified hub address 
//  in  eeprom
//
//Arguments:
//  (IN) HUB_ADDR -- address of the hub the the ID belongs to
//  (IN) ID -- message ID to store
//
//Return:
//  void
//============================================================================
void eepromStoreMsgId(const byte HUB_ADDR, const byte ID)
{
    eepromWriteSingle(ID, getAddrIndx(HUB_ADDR) 
            + GRAPH_ARRAY_MEM_START);
}


//============================================================================
//Description: Calculates a three digit message ID for the given hub address
//
//Arguments: 
//  (IN) -- HUB_ADDR -- address of the hub that needs the message ID
//
//Return:
//  byte -- 3 digit message ID
//============================================================================
byte generateMsgId(const byte HUB_ADDR)
{
    byte msgId;
    
    do
    {
        srand(millis());
        msgId = rand() % 255;
    }while(msgId == eepromRetrieveMsgId(HUB_ADDR) 
            || msgId == 0);

    eepromStoreMsgId(HUB_ADDR, msgId);

    return msgId;
}
