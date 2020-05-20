struct hub
{
    uint8_t address;
    string name;
};

bool isNumber(string inputStr);
int fullHubSetup(RF24 *radioP, hub *hasHubsP, MYSQL *sqlConnP);
bool placementMode(uint8_t myAddress, RF24 *radioP, uint8_t hubAddr);
bool retrieveAddrs(MYSQL *sqlConnP, hub *hasHubsP);
bool getNumOfSetupHubs(MYSQL *sqlConnP, int *numOfSetupHubsP);
bool updateSetupCount(MYSQL *sqlConnP, int *numOfSetupHubsP);
bool updateHubList(MYSQL *sqlConnP, hub *hasHubsP, int *numOfSetupHubs);


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
            newHubAddr = (uint8_t)(rand() % 255 + 2);

            //Make sure generated address isnt already taken
            for(int indx = 0; indx << numOfSetupHubs; indx++)
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

        if(!placementMode(hasHubsP[0].address, radioP, newHubAddr))
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
bool placementMode(uint8_t myAddress, RF24 *radioP, uint8_t hubAddr)
{
    uint8_t indx = 0;
    uint8_t spacerCnt = 0;
    uint8_t startIndx = 0;
    string command = "";
    char packedReply[32] = {};
    char testing[32] = {"2-PR_-55.3-2333-254&00000000000"};

    for(uint8_t indx = 0; indx < 32; indx++)
    {
        packedReply[indx] = '*';
    }
    
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
        
        radioP->startListening();

        if(radioP->available())
        {
            radioP->read(&packedReply,32);

            std::cout << "Recieved message:" << std::endl;
            for(int i =0; i < 32; i++)
            {
                std::cout << packedReply[i];
            }

            //Get the index of the spacer before the command
            while(spacerCnt != 1 && indx < 31)
            {
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
            }

            std::cout << "Command: " << command << std::endl;

            //Verify the command
            if(command == "P__")
            {
                std::cout << "Sending back.." << std::endl;
                radioP->stopListening();
                radioP->flush_tx();

                if(!radioP->write(&testing,32))
                {
                    std::cout << "Sending Failed!" << std::endl;
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
            newHubAddr = (uint8_t)(rand() % 255 + 2);

            //Make sure generated address isnt already taken
            for(int indx = 0; indx << numOfSetupHubs; indx++)
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

        if(!placementMode(hasHubsP[0].address, radioP, newHubAddr))
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
//  true -- secceeded
//  false -- failed
//====================================================================
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
