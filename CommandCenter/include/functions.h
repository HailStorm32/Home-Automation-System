struct hub
{
    uint8_t address;
    string name;
    uint8_t lastUsedId;
};

bool isNumber(string inputStr);
int fullHubSetup(RF24 *radioP, hub *hasHubsP, MYSQL *sqlConnP);
bool placementMode(uint8_t myAddress, RF24 *radioP, uint8_t hubAddr,
        hub *hasHubsP);
uint8_t getAddrIndx(uint8_t address, hub * hasHubsP);
bool unpackMessage(char *packedMessage, uint8_t *fromAddress, uint8_t *toAddress, 
        float *temperature, short *motionCount, char *command, bool *duplicateMsg
        , hub *hasHubsP);
uint8_t findNodeWithShortestPath(uint8_t targetNode, uint8_t startingNode, hub *hasHubsP);
uint8_t generateMsgId(const uint8_t HUB_ADDR);
bool packMessage(char *command, char *packedMessage, uint8_t targetAddr, 
        uint8_t senderAddr, float tempData, short motionData, hub *hasHubsP);
bool retrieveAddrs(MYSQL *sqlConnP, hub *hasHubsP);
bool getNumOfSetupHubs(MYSQL *sqlConnP, int *numOfSetupHubsP);
bool updateSetupCount(MYSQL *sqlConnP, int *numOfSetupHubsP);
bool updateHubList(MYSQL *sqlConnP, hub *hasHubsP, int *numOfSetupHubs);
bool removeTestHubs(MYSQL *sqlConnP);
int testHubSetup(RF24 *radioP, hub *hasHubsP, MYSQL *sqlConnP);


//=====================================================================
//Description: Checks to see if given string is all numbers by checking
//  each char in the string
//
//Arguments:
//  (IN) inputStr -- string to search
//
//Return:
//  true -- string is all numbers
//  false -- string contains a non number
//=====================================================================
bool isNumber(string inputStr)
{
    for(uint8_t indx = 0; indx < inputStr.length(); indx++)
    {
        //If char at given indx is not equal to the ASCII values of 0-9
        if(!(inputStr[indx] >= 48 && inputStr[indx] <= 57))
        {
            return false;
        }
    }
    return true;
}


//======================================================================
//Description: Gives hub addresses of other hubs as well as its own. Also 
//  does a placment setup that will constantly ping the hub and report 
//  if the server and the hub have a comms link
//
//Arguments:
//  (IN) radioP -- pointer to radio instance
//  (IN) hasHubsP -- pointer to struct that contains the hub list
//  (IN) sqlConnP -- pointer to sql connection
//
//Return:
//  0 -- if setup succeeded
//  1 -- if setup failed 
//  2 -- user requested exit
//=======================================================================
int fullHubSetup(RF24 *radioP, hub *hasHubsP, MYSQL *sqlConnP)
{
    const uint8_t DEFAULT_HUB_ADDR = 2;

    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    int  numOfSetupHubs;
    int  numOfHubs2Setup;
    uint8_t newHubAddr = 0;
    uint8_t addresses2Send[MAX_NUM_OF_ADDRS];
    bool exitFlag = false;
    string usrInput;
    string newHubName;

    //Initilize array
    for(uint8_t indx = 0; indx < MAX_NUM_OF_ADDRS; indx++)
    {
        addresses2Send[indx] = 0;
    }
    
    //Get number of hubs setup from database
    if(!getNumOfSetupHubs(sqlConnP, &numOfSetupHubs))
    {
        return 1;
    }

    system("clear");

    //Get and varify number of hubs to setup from user
    while(!exitFlag)
    {
        std::cout << "\nEnter how many hubs will be setup: ";
        std::cin >> usrInput;

        //Allow user to quit
        if(usrInput[0] == 'q' || usrInput[0] == 'Q')
        {
            system("clear");
            return 2;
        }

        if(!isNumber(usrInput))
        {
            std::cout << "WARNING: Entered value is not a number." << std::endl;
            numOfHubs2Setup = 1;
        }
        else
        {
            numOfHubs2Setup = std::stoi(usrInput);
            exitFlag = true;
        }
        
        if(numOfHubs2Setup <= 0)
        {
            std::cout << "WARNING: Number of hubs to set up needs to be "
                "greater than 0"  << std::endl;
            exitFlag = false;
        }
        
        if((numOfSetupHubs + numOfHubs2Setup) > MAX_NUM_OF_ADDRS)
        {
            std::cout << "WARNING: Number of hubs to set up will exceed max number"
                " of allowable setup hubs.\n Exceeds " <<  MAX_NUM_OF_ADDRS <<
                " limit by: " << ((numOfSetupHubs + numOfHubs2Setup) 
                        - MAX_NUM_OF_ADDRS) << std::endl;
            exitFlag = false;
        }
    }
         
    radioP->begin(); 
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    radioP->setRetries(1, 15);
    radioP->setChannel(124);

    //Setup each hub
    for(uint8_t indx = 0; (int)indx < numOfHubs2Setup; indx++)
    {
        system("clear");
        std::cout << "Entering setup of hub " << (int)indx + 1 << " of "
            << numOfHubs2Setup << "." << std::endl;
        
        //Get a name for the hub
        exitFlag = false;
        while(!exitFlag)
        {
            exitFlag = true;
            std::cout << "\n\nPlease enter a name for hub " << (int)indx + 1 
                << " of " << numOfHubs2Setup << ": ";
            cin >> usrInput;

            if(usrInput.length() > 20)
            {
                system("clear");
                std::cout << "ERROR: Name exceeds 20 char limit!" << std::endl;
                exitFlag = false;
            }
            else
            {
                newHubName = usrInput;
                std::cout << "Hub name is: " << newHubName << std::endl;
            }
        }

        if(!retrieveAddrs(sqlConnP, hasHubsP))
        {
            return 1;
        }

        //Get number of hubs setup from database
        if(!getNumOfSetupHubs(sqlConnP, &numOfSetupHubs))
        {
            return 1;
        }

        std::cout << "\nCreating address..." << std::endl;
        
        exitFlag = false;
        while(!exitFlag)
        {
            exitFlag = true;

            do
            {
                srand(time(NULL));
                newHubAddr = rand() % 255;
            }while(newHubAddr == 0);

            //Make sure generated address isnt already taken
            for(int indx = 0; indx < numOfSetupHubs; indx++)
            {
                if(newHubAddr == hasHubsP[indx].address)
                {
                    exitFlag = false;
                    break;
                }
            }
        }
         
        //Update the database with the info we just got
        numOfSetupHubs++;
        hasHubsP[numOfSetupHubs - 1].address = newHubAddr;
        hasHubsP[numOfSetupHubs - 1].name = newHubName;

        std::cout << "Updating database..." << std::endl; 
        if(!updateSetupCount(sqlConnP, &numOfSetupHubs) || 
                !updateHubList(sqlConnP, hasHubsP, &numOfSetupHubs))
        {return 1;}

        std::cout << "Address for hub is: " << (int)newHubAddr << std::endl; 

        //Get addresses ready for sending
        //Hub needs addresses in an array following this format
        //myAddr, otherHubAddrs, .. .., serverAddr
        addresses2Send[0] = newHubAddr;
        addresses2Send[MAX_NUM_OF_ADDRS - 1] = hasHubsP[0].address;

        for(int i=0; i < MAX_NUM_OF_ADDRS; i++)
        {
            std::cout << (int)hasHubsP[i].address << " " << std::endl;
        }
        
        if(numOfSetupHubs > 1)
        {
            for(uint8_t indx = 1; indx < (numOfSetupHubs - 1); indx++)
            {
                addresses2Send[indx] = hasHubsP[indx].address;
            }
        }

        system("clear");
        std::cout << "\nPlease hold the button on your hub until the"
            " status LED turns solid." 
            "\n\nPress [ENTER] once complete." << std::endl;
        std::cin.ignore();
        std::cin.get();

        //TODO: Send new address to hub as well as the rest of the addresses
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "\n\nPreparing to send addresses to " 
            << newHubName << " hub..." << std::endl; 

        radioP->openWritingPipe(DEFAULT_HUB_ADDR);
        radioP->stopListening();

        std::cout << "Sending..." << std::endl;

        if(!radioP->write(&addresses2Send,MAX_NUM_OF_ADDRS,0))
        {
            std::cout << "ERROR: Cant send addresses to " << newHubName
                << " hub!" << std::endl;
            return 1;
        }
        
        std::cout << "\nSent!" << std::endl;

        if(!placementMode(hasHubsP[0].address, radioP, newHubAddr, hasHubsP))
        {
            return 1;
        }
    }
    system("clear");
    return 0;
}


//=====================================================================
//Description: Begins placement mode. Server will continuously ping hub
//  and report whether the hub is in range or not.
//
//Arguments:
//  (IN) sqlConnP -- pointer to sql connection
//  (IN) radioP -- pointer to radio 
//
//Return:
//  true -- placment succeeded
//  false -- placement failed
//=====================================================================
bool placementMode(uint8_t myAddress, RF24 *radioP, uint8_t hubAddr,
        hub *hasHubsP)
{
    uint8_t indx = 0;
    uint8_t spacerCnt = 0;
    uint8_t startIndx = 0;
    string command = "";
    char packedReply[32] = {};
    char packedMsg[32];
    bool hasSent = false;

    uint8_t fromAddr = 0;
    uint8_t toAddr = 0;
    float temp = 0;
    short motionCnt = 0;
    char commandCstr[COMMAND_SIZE];
    bool duplicateMsg;

    for(uint8_t indx = 0; indx < 32; indx++)
    {
        packedReply[indx] = '*';
        packedMsg[indx] = '*';
    }

    packMessage("PR_", packedMsg, hubAddr, myAddress, 0, 0, hasHubsP);

    radioP->openWritingPipe(hubAddr);
    radioP->openReadingPipe(1, myAddress);

    system("clear");
    std::cout << "\nEntered placement mode." << std::endl;
    std::cout << "\nOnce the process begins, place the hub where you "
        "want it. Note the red and blue LEDs."
        "\nRED = Can NOT reach server, place elsewhere"
        "\nBLUE = Ok to place"
        "\n\n Once you have found a place, and the LED is blue,"
        "Press and hold the button until the blue LED starts to flash."
        << std::endl;
    std::cout << "\n\nStarting...";

    while(command != "PD_")
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        spacerCnt =0;
        indx = 0;
        command = "";
        hasSent = false;
        
        radioP->startListening();

        if(radioP->available())
        {
            radioP->read(&packedReply,32);

            std::cout << "Recieved message:" << std::endl;
            for(int i =0; i < 32; i++)
            {
                std::cout << packedReply[i];
            }
            std::cout << std::endl;

            //TODO: check to see if we recived this message already
            
            unpackMessage(packedReply, &fromAddr, &toAddr, &temp, &motionCnt, 
                    commandCstr, &duplicateMsg, hasHubsP);

            command = commandCstr;
            
            if(duplicateMsg)
            {
                std::cout << "Recieved duplicate message! Ignoring.." 
                    << std::endl;
                continue;
            }

           /* //Get the index of the spacer before the command
            while(spacerCnt != 1 && indx < 31)
            {
                radioP->openWritingPipe(hubAddr);
                if(packedReply[indx] == '-')
                {
                    spacerCnt++;
                }
                else
                {
                    indx++;
                }
            }

            indx++;

            //Store the command
            startIndx = indx;
            for(uint8_t indx2 = startIndx; packedReply[indx2] != '-'; indx2++)
            {
                command += packedReply[indx2];
            }*/

            std::cout << "Command: " << command << std::endl;

            //Verify the command
            if(command == "P__")
            {
                std::cout << "Sending back.." << std::endl;
                radioP->stopListening();
                radioP->flush_tx();
                indx = 1;
                bool hasSent = false;

                if(!radioP->write(&packedMsg,32))
                {
                    std::cout << "target send Failed!" << std::endl;
                
                while(!hasSent)
                {
                    //Try and forward to other hubs
                    while(indx < MAX_NUM_OF_ADDRS && 
                            hasHubsP[indx].address != 0)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                        radioP->openWritingPipe(hasHubsP[indx].address);
                        if(!radioP->write(&packedMsg,32))
                        {
                            std::cout << (int)hasHubsP[indx].address << " send Failed!" << std::endl;
                        }
                        else{hasSent = true;}
                        indx++;
                    }
                    indx = 1;
                }
                }
                radioP->txStandBy();
            }
        }
    }
    system("clear");
    std::cout << "\nPlacement done!" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    return true;
}


//=========================================================================
//Description: returns what index the given address is stored in the
//  address array
//
//Arguments:
//  (IN) address -- address to get the index for
//
//Returns:
//  uint8_t -- the index for the given address
//=========================================================================
uint8_t getAddrIndx(uint8_t address, hub * hasHubsP)
{
    uint8_t indx = 0;

    while(indx != MAX_NUM_OF_ADDRS && hasHubsP[indx].address != address)
    {
        indx++;
    }

    if(indx >= MAX_NUM_OF_ADDRS)
    {
        std::cout << "Error in 'getAddrIndx'" << std::endl;
        while(true){}
    }
    return indx;
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
//  (IN) hasHubsP -- struct containing hub info
//
//Return:
//  true -- was able to unpack the command
//  false -- unpacking failed
//=====================================================================
bool unpackMessage(char *packedMessage, uint8_t *fromAddress, uint8_t *toAddress, 
        float *temperature, short *motionCount, char *command, bool *duplicateMsg
        , hub *hasHubsP)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> messageID -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, -, 1, 4, 0,&]
	
    255-S-70.3-12-245-140&
    */

    uint8_t indx = 0;
    uint8_t stringHolderIndx = 0;
    uint8_t messageId = 0;
    char stringHolder[11];
    
    for(uint8_t indx2 = 0; indx2 < 10; indx2++)
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
    for(uint8_t indx2 = 0; indx2 < 10; indx2++)
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
    for(uint8_t indx2 = 0; indx2 < 10; indx2++)
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
    for(uint8_t indx2 = 0; indx2 < 10; indx2++)
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
    for(uint8_t indx2 = 0; indx2 < 10; indx2++)
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
    for(uint8_t indx2 = 0; indx2 < 10; indx2++)
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

    if(messageId == hasHubsP[getAddrIndx(*fromAddress, hasHubsP)].lastUsedId)
    {
        *duplicateMsg = true;
    }
    else
    {
        *duplicateMsg = false;
        hasHubsP[getAddrIndx(*fromAddress, hasHubsP)].lastUsedId = messageId;
    }

    return true;
}


//=========================================================================
//Description: Traverses address graph using BFS. Then using the list of
//  parent nodes, goes backwards from the target node to find the node that
//  starts the shortest path
//
//Arguments:
//  (IN) targetNode -- contains the address of the target node
//  (IN) startingNode -- contains the address of the starting node
//  (IN) hasHubsP -- struct containing hub info
//
//Returns:
//  0 -- if failed
//  2-255 -- address of the node that starts the path
//=========================================================================
uint8_t findNodeWithShortestPath(uint8_t targetNode, uint8_t startingNode, hub *hasHubsP)
{
    bool haveVisited[MAX_NUM_OF_ADDRS];
    const uint8_t myAddress = hasHubsP[0].address;
    uint8_t currentNode = 0;
    uint8_t indx;
    uint8_t adjNode;
    queue<uint8_t> graphQueue;

    for(uint8_t indx = 0; indx < MAX_NUM_OF_ADDRS; indx++)
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
    haveVisited[getAddrIndx(currentNode, hasHubsP)] = true;

    while(!graphQueue.empty())
    {
        graphQueue.pop();

        //Queue all adjacent nodes
        while(addrGraph[getAddrIndx(currentNode, hasHubsP)].adjNodes[indx] != 0)
        {
            adjNode = addrGraph[getAddrIndx(currentNode, hasHubsP)].adjNodes[indx];

            if(haveVisited[getAddrIndx(adjNode, hasHubsP)] == false)
            {
                //Add to queue
                graphQueue.push(adjNode);

                //Mark as visited
                haveVisited[getAddrIndx(adjNode, hasHubsP)] = true;

                //Add its parent node
                addrGraph[getAddrIndx(adjNode, hasHubsP)].parentNode = currentNode;

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

    while(addrGraph[getAddrIndx(currentNode, hasHubsP)].parentNode != myAddress &&
            indx < MAX_NUM_OF_ADDRS)
    {
        currentNode = addrGraph[getAddrIndx(currentNode, hasHubsP)].parentNode;
        indx++;
    }
    if(indx >= MAX_NUM_OF_ADDRS)
    {
        return 0;
    }
    else
    {
        return currentNode;
    }
}


//========================================================================
//Description: sends a message to the given address using the address
//  graph (if it has been set up) to send out messages
//
//Arguments:
//  (IN) radioP -- pointer to radio instance
//  (IN) receivingAddr -- address to send message to
//  (IN) motionCnt -- number of motion triggers
//  (IN) temperature -- room temperature
//  (IN) command -- command
//  (IN) hasHubsP -- struct containing hub info
//
//Return:
//  true -- was able to send to a hub
//  false -- wasnt able to send to any hub
//=========================================================================
bool sendMessage(RF24 *radioP, uint8_t receivingAddr, short motionCnt, 
        float const * const temperature, char *command, hub *hasHubsP)
{
    uint8_t myAddress = hasHubsP[0].address;
    char packedMessage[32];
    uint8_t addrIndx = 1;
    bool messageSent = false;
    uint8_t hub2SendTo;

    radioP->stopListening();
    radioP->openWritingPipe(receivingAddr);

    packMessage(command, packedMessage, receivingAddr, myAddress, *temperature,
           motionCnt, hasHubsP);

    if(radioP->write(packedMessage, 32))
    {
        messageSent = true;
    }
    else
    {
        if(hasGraph == true)
        {
            uint8_t indx = 0;
            hub2SendTo = findNodeWithShortestPath(receivingAddr, myAddress, 
                    hasHubsP);
            
            if(!radioP->write(packedMessage, 32))
            {
                //try and see if there is a path from another adjacent hub
                while(indx < MAX_NUM_OF_ADDRS && 
                        addrGraph[getAddrIndx(myAddress, hasHubsP)].adjNodes[indx] != 0)
                {
                    hub2SendTo = findNodeWithShortestPath(
                            addrGraph[getAddrIndx(myAddress, hasHubsP)]
                            .adjNodes[indx], myAddress, hasHubsP);

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
            while(hasHubsP[addrIndx].address != 0 && addrIndx != 
                    MAX_NUM_OF_ADDRS)
            {
                radioP->openWritingPipe(hasHubsP[addrIndx].address);
                if(radioP->write(packedMessage, 32))
                {
                    messageSent = true;
                    break;
                }
                addrIndx++;
            }
        }
    }
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


//============================================================================
//Description: Calculates a three digit message ID for the given hub address
//
//Arguments: 
//  (IN) HUB_ADDR -- address of the hub that needs the message ID
//  (IN) hasHubsP -- struct containing hub info
//
//Return:
//  uint8_t -- 3 digit message ID
//============================================================================
uint8_t generateMsgId(const uint8_t HUB_ADDR, hub *hasHubsP)
{
    uint8_t msgId;
    
    do
    {
        srand(time(NULL));
        msgId = rand() % 255;
    }while(msgId == hasHubsP[getAddrIndx(HUB_ADDR, hasHubsP)].lastUsedId 
            || msgId == 0);

    hasHubsP[getAddrIndx(HUB_ADDR, hasHubsP)].lastUsedId = msgId;

    return msgId;
}


//=========================================================================
//Description: Packs all the data needed to be sent into a 32 char array
//
//Arguments:
//  (IN) targetAddr -- single uint8_t address of where this packet is going
//  (IN) senderAddr -- single uint8_t address of sender address
//  (IN) tempData -- data from the temperature sensor
//  (IN) motionData -- data from the motion sensor
//  (IN) command -- 1-3 char command
//  (IN) packedMessage -- char array containing the packed message
//
//Return:
//  true -- was packed
//==========================================================================
bool packMessage(char *command, char *packedMessage, uint8_t targetAddr, 
        uint8_t senderAddr, float tempData, short motionData, hub *hasHubsP)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> messageID -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, -, 1, 4, 0,&]
	
    255-S-70.3-12-245-140&
    */

    char packedMsg [32] = {};
    string strNumHolder;
    //uint8_t numHolderIndx = 0;
    //String packedStr;
    uint8_t indxCounter = 0; 
    uint8_t indxStart = 0;
    uint8_t genIndx = 0;

    for(uint8_t indx = 0; indx < 32; indx++)
    {
        packedMsg[indx] = '0';
        packedMessage[indx] = '0';
    }

    //Add targetAddr to message array
    strNumHolder = to_string(targetAddr);
    std::cout << "targAddr: " << strNumHolder << std::endl;
    while(genIndx < strNumHolder.length())
    {
        packedMsg[genIndx] = strNumHolder[genIndx];
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

    //Add tempData to message array
    strNumHolder = to_string(tempData);
    indxStart = indxCounter; 
    while(genIndx < strNumHolder.length())
    {
        packedMsg[genIndx + indxStart] = strNumHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;
    
    //Add motion to message array
    strNumHolder = to_string(motionData);
    indxStart = indxCounter; 
    while(genIndx < strNumHolder.length())
    {
        packedMsg[genIndx + indxStart] = strNumHolder[genIndx];
        indxCounter++;
        genIndx++;
    }
     
    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;

    //Add senderAddr to message array
    strNumHolder = to_string(senderAddr);
    indxStart = indxCounter; 
    while(genIndx < strNumHolder.length())
    {
        packedMsg[genIndx + indxStart] = strNumHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    genIndx = 0;

    strNumHolder = to_string(generateMsgId(senderAddr, hasHubsP)); 
    indxStart = indxCounter; 
    while(genIndx < strNumHolder.length())
    {
        packedMsg[genIndx + indxStart] = strNumHolder[genIndx];
        indxCounter++;
        genIndx++;
    }

    //Add end char
    packedMsg[indxCounter] = '&';

    
    for(uint8_t indx = 0; indx < 32; indx++)
    {
        packedMessage[indx] = packedMsg[indx];
    }


    std::cout << "In function: ";
    for(uint8_t indx = 0; indx < 32; indx++)
    {
        std::cout << packedMessage[indx];
    }
    std::cout << std::endl;

    return true;
}


//======================================================================
//Description: Retrieves hub addresses from database
//
//Arguments:
//  (IN) sqlConnP -- pointer to mysql connetion
//  (OUT) hasHubsP -- pointer to struct that holds hub data
//
//Return:
//  true -- succeeded
//  false -- failed
//=====================================================================
bool retrieveAddrs(MYSQL *sqlConnP, hub *hasHubsP)
{
    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    int genIndx = 0;

    if(mysql_ping(sqlConnP))
    {
       std::cout << "\nERROR: Ping to database from 'retrieveAddrs' failed!"
           << std::endl;
       mysql_close(sqlConnP);
       return false;
    }

    //Get addresses from database
    if(mysql_query(sqlConnP, "SELECT * FROM hubs") && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query in 'retrieveAddrs' failed!" << std::endl;
        std::cout << mysql_error(sqlConnP) << endl;
        mysql_close(sqlConnP);
        return false;
    }
    sqlResult = mysql_store_result(sqlConnP);

    while((sqlRow = mysql_fetch_row(sqlResult)) != NULL)
    {
        if(genIndx >= 255)
        {
            std::cout << "\nWARNING: Number of addresses in database exceeds 255!" 
                " Some addresses may be missing!" << std::endl;
            mysql_free_result(sqlResult);
            break;
        }

        hasHubsP[genIndx].address = (uint8_t)std::stoi(sqlRow[0]);
        hasHubsP[genIndx].name = sqlRow[1];
        genIndx++;
    }
    mysql_free_result(sqlResult);
    return true;
}


//=====================================================================
//Description: Gets the number of hubs that have been setup from 
//  the database
//
//Agruments:
//  (IN) sqlConnP -- pointer to mysql connection
//  (OUT) numOfSetupHubsP -- pointer to variable that will hold result
//
//Return:
//  true -- succeeded
//  false -- failed
//====================================================================
bool getNumOfSetupHubs(MYSQL *sqlConnP, int *numOfSetupHubsP)
{
    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    string sqlQuery;

    if(mysql_ping(sqlConnP))
    {
       std::cout << "\nERROR: Ping to database from 'getNumOfSetupHubs' failed!"
           << std::endl;
       std::cout << mysql_error(sqlConnP) << endl;
       mysql_close(sqlConnP);
       return false;
    }
    
    sqlQuery = "SELECT * FROM variables WHERE name='numOfSetupHubs'";
    if(mysql_query(sqlConnP, sqlQuery.c_str()) 
            && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query failed to get 'numOfSetupHubs'!" 
            << std::endl;
        std::cout << mysql_error(sqlConnP) << endl;
        mysql_close(sqlConnP);
        return false;
    }
    sqlResult = mysql_store_result(sqlConnP);
    sqlRow = mysql_fetch_row(sqlResult);
    
    *numOfSetupHubsP = std::stoi(sqlRow[1]);
    mysql_free_result(sqlResult);
    
    return true;
}


//====================================================================
//Description:  Update the count of setup hubs in the database
//
//Arguments:
//  (IN) sqlConnP -- pointer to sql connection
//  (IN) numOfSetupHubs -- pointer to variable that contains setup count
//
//Return:
//  true -- secceeded
//  false -- failed
//====================================================================
bool updateSetupCount(MYSQL *sqlConnP, int *numOfSetupHubsP)
{
    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    string sqlQuery;

    if(mysql_ping(sqlConnP))
    {
       std::cout << "\nERROR: Ping to database from 'updateSetupCount' failed!"
           << std::endl;
       std::cout << mysql_error(sqlConnP) << endl;
       mysql_close(sqlConnP);
       return false;
    }
    
    sqlQuery = "UPDATE variables SET value='" + std::to_string(*numOfSetupHubsP) 
        + "' WHERE name='numOfSetupHubs'";
    if(mysql_query(sqlConnP, sqlQuery.c_str()) 
            && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query failed to update 'numOfSetupHubs'!" 
            << std::endl;
        std::cout << mysql_error(sqlConnP) << endl;
        mysql_close(sqlConnP);
        return false;
    }
    return true;
}


//====================================================================
//Description:  Update the hub list in the database
//
//Arguments:
//  (IN) sqlConnP -- pointer to sql connection
//  (IN) hasHubsP -- pointer to struct containing hub data
//  (IN) numOfSetupHubsP -- pointer to variable that contains setup count
//
//Return:
//  true -- secceeded
//  false -- failed
//====================================================================
bool updateHubList(MYSQL *sqlConnP, hub *hasHubsP, int *numOfSetupHubsP)
{
    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    string sqlQuery;

    if(mysql_ping(sqlConnP))
    {
       std::cout << "\nERROR: Ping to database from 'updateHubList' failed!"
           << std::endl;
       mysql_close(sqlConnP);
       return false;
    }
    
    sqlQuery = "INSERT INTO hubs VALUES('" + 
        std::to_string(hasHubsP[*numOfSetupHubsP-1].address) 
        + "', '" + hasHubsP[*numOfSetupHubsP-1].name + "')";
    std::cout << sqlQuery << std::endl;
    if(mysql_query(sqlConnP, sqlQuery.c_str()) 
            && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query failed to update hub list!" 
            << std::endl;
        std::cout << mysql_error(sqlConnP) << endl;
        mysql_close(sqlConnP);
        return false;
    }
    return true;
}


//==================================================================
//Description: Sets up test hubs. A quicker version of fullHubSetup
//
//Arguments:
//  (IN) radioP -- pointer to radio instance
//  (IN) hasHubsP -- pointer to struct that contains the hub list
//  (IN) sqlConnP -- pointer to sql connection
//
//Return:
//  0 -- if setup succeeded
//  1 -- if setup failed 
//  2 -- user requested exit
//=================================================================
int testHubSetup(RF24 *radioP, hub *hasHubsP, MYSQL *sqlConnP)
{
    const uint8_t DEFAULT_HUB_ADDR = 2;

    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    int  numOfSetupHubs;
    int  numOfHubs2Setup;
    uint8_t newHubAddr = 0;
    uint8_t addresses2Send[MAX_NUM_OF_ADDRS];
    bool exitFlag = false;
    string usrInput;
    string newHubName;

    //Initilize array
    for(uint8_t indx = 0; indx < MAX_NUM_OF_ADDRS; indx++)
    {
        addresses2Send[indx] = 0;
    }
    
    //Get number of hubs setup from database
    if(!getNumOfSetupHubs(sqlConnP, &numOfSetupHubs))
    {
        return 1;
    }

    system("clear");

    //Get and varify number of hubs to setup from user
    while(!exitFlag)
    {
        std::cout << "\nEnter how many hubs will be setup: ";
        std::cin >> usrInput;

        //Allow user to quit
        if(usrInput[0] == 'q' || usrInput[0] == 'Q')
        {
            system("clear");
            return 2;
        }

        if(!isNumber(usrInput))
        {
            std::cout << "WARNING: Entered value is not a number." << std::endl;
            numOfHubs2Setup = 1;
        }
        else
        {
            numOfHubs2Setup = std::stoi(usrInput);
            exitFlag = true;
        }
        
        if(numOfHubs2Setup <= 0)
        {
            std::cout << "WARNING: Number of hubs to set up needs to be "
                "greater than 0"  << std::endl;
            exitFlag = false;
        }
        
        if((numOfSetupHubs + numOfHubs2Setup) > MAX_NUM_OF_ADDRS)
        {
            std::cout << "WARNING: Number of hubs to set up will exceed max number"
                " of allowable setup hubs.\n Exceeds " <<  MAX_NUM_OF_ADDRS <<
                " limit by: " << ((numOfSetupHubs + numOfHubs2Setup) 
                        - MAX_NUM_OF_ADDRS) << std::endl;
            exitFlag = false;
        }
    }
         
    radioP->begin(); 
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    radioP->setRetries(1, 15);
    radioP->setChannel(124);

    //Setup each hub
    for(uint8_t indx = 0; (int)indx < numOfHubs2Setup; indx++)
    {
        system("clear");
        std::cout << "Entering setup of hub " << (int)indx + 1 << " of "
            << numOfHubs2Setup << "." << std::endl;
        
        //Get a name for the hub
        newHubName = "test";

        if(!retrieveAddrs(sqlConnP, hasHubsP))
        {
            return 1;
        }

        //Get number of hubs setup from database
        if(!getNumOfSetupHubs(sqlConnP, &numOfSetupHubs))
        {
            return 1;
        }

        std::cout << "\nCreating address..." << std::endl;
        
        exitFlag = false;
        while(!exitFlag)
        {
            exitFlag = true;

            do
            {
                srand(time(NULL));
                newHubAddr = rand() % 255;
            }while(newHubAddr == 0);

            //Make sure generated address isnt already taken
            for(int indx = 0; indx < numOfSetupHubs; indx++)
            {
                if(newHubAddr == hasHubsP[indx].address)
                {
                    exitFlag = false;
                    break;
                }
            }
        }
         
        //Update the database with the info we just got
        numOfSetupHubs++;
        hasHubsP[numOfSetupHubs - 1].address = newHubAddr;
        hasHubsP[numOfSetupHubs - 1].name = newHubName;

        std::cout << "Updating database..." << std::endl; 
        if(!updateSetupCount(sqlConnP, &numOfSetupHubs) || 
                !updateHubList(sqlConnP, hasHubsP, &numOfSetupHubs))
        {return 1;}

        std::cout << "Address for hub is: " << (int)newHubAddr << std::endl; 

        //Get addresses ready for sending
        //Hub needs addresses in an array following this format
        //myAddr, otherHubAddrs, .. .., serverAddr
        addresses2Send[0] = newHubAddr;
        addresses2Send[MAX_NUM_OF_ADDRS - 1] = hasHubsP[0].address;

        for(int i=0; i < MAX_NUM_OF_ADDRS; i++)
        {
            std::cout << (int)hasHubsP[i].address << " " << std::endl;
        }
        
        if(numOfSetupHubs > 1)
        {
            for(uint8_t indx = 1; indx < (numOfSetupHubs - 1); indx++)
            {
                addresses2Send[indx] = hasHubsP[indx].address;
            }
        }

        system("clear");
        std::cout << "\nPlease hold the button on your hub until the"
            " status LED turns solid." 
            "\n\nPress [ENTER] once complete." << std::endl;
        std::cin.ignore();
        std::cin.get();

        //TODO: Send new address to hub as well as the rest of the addresses
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "\n\nPreparing to send addresses to " 
            << newHubName << " hub..." << std::endl; 

        radioP->openWritingPipe(DEFAULT_HUB_ADDR);
        radioP->stopListening();

        std::cout << "Sending..." << std::endl;

        if(!radioP->write(&addresses2Send,MAX_NUM_OF_ADDRS,0))
        {
            std::cout << "ERROR: Cant send addresses to " << newHubName
                << " hub!" << std::endl;
            return 1;
        }
        
        std::cout << "\nSent!" << std::endl;

        if(!placementMode(hasHubsP[0].address, radioP, newHubAddr, hasHubsP))
        {
            return 1;
        }
    }
    system("clear");
    return 0;
}


//=======================================================================
//Description: Remove test setups from database
//
//Arguments:
//  (IN) sqlConnP -- pointer to sql connection
//
//Return:
//  true -- succeeded
//  false -- failed
//========================================================================
bool removeTestHubs(MYSQL *sqlConnP)
{
    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;
    string sqlQuery;

    if(mysql_ping(sqlConnP))
    {
       std::cout << "\nERROR: Ping to database from 'removeTestHubs' failed!"
           << std::endl;
       mysql_close(sqlConnP);
       return false;
    }
    
    sqlQuery = "UPDATE variables SET value='1' WHERE name='numOfSetupHubs'";
    std::cout << sqlQuery << std::endl;
    if(mysql_query(sqlConnP, sqlQuery.c_str()) 
            && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query failed to update list!" 
            << std::endl;
        std::cout << mysql_error(sqlConnP) << endl;
        mysql_close(sqlConnP);
        return false;
    }

    
    sqlQuery = "DELETE FROM hubs WHERE name='test'";
    std::cout << sqlQuery << std::endl;
    if(mysql_query(sqlConnP, sqlQuery.c_str()) 
            && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query failed to update list!" 
            << std::endl;
        std::cout << mysql_error(sqlConnP) << endl;
        mysql_close(sqlConnP);
        return false;
    }

    return true;
}


//=========================================================================
//Description: Requests all the hubs for a list of hubs that are adj
//  to them. Then create a graph and send it to all the hubs
//
//Arguments:
//  (IN) sqlConnP -- pointer to sql connection
//  (IN) radioP -- pointer to radio instance
//  (IN) hasHubsP -- pointer to struct that holds hub data
//
//Return:
//  true -- graph was made
//  false -- graph was not made
//==========================================================================
bool buildAndSendGraph(MYSQL *sqlConnP, RF24 *radioP, hub *hasHubsP)
{
    struct node
    {
        uint8_t adjNodes[MAX_NUM_OF_ADDRS];
        uint8_t parentNode;
    };

    node addrGraph[MAX_NUM_OF_ADDRS];
    int numOfSetupHubs;
    

    getNumOfSetupHubs(sqlConnP, &numOfSetupHubs);
    retrieveAddrs(sqlConnP, hasHubsP);

    //Zero out the addrGraph
    for(uint8_t nodeIndx = 0; nodeIndx < MAX_NUM_OF_ADDRS; nodeIndx++)
    {
        for(uint8_t addrIndx = 0; addrIndx < MAX_NUM_OF_ADDRS; addrIndx++)
        {
            addrGraph[nodeIndx].adjNodes[addrIndx] = 0;
        }
        addrGraph[nodeIndx].parentNode = 0;
    }

    //Request and recieve the ajd list from all setup hubs
    for(uint8_t indx = 0; indx < numOfSetupHubs - 1; indx++)
    {
      //TODO: Need to complete  
    }

}
