struct hub
{
    uint8_t address;
    string name;
};

bool hubSetup(RF24 *radioP, uint8_t numOfHubSetups);
bool retrieveAddrs(MYSQL *sqlConnP, hub *hasHubsP);


//======================================================================
//Description: Gives hub addresses of other hubs as well as its own. Also 
//  does a placment setup that will constantly ping the hub and report 
//  if the server and the hub have a comms link
//
//Arguments:
//  (IN) radioP -- pointer to radio instance
//
//Return:
//  true -- if setup succeeded
//  false -- if setup failed 
//=======================================================================
bool hubSetup(RF24 *radioP, uint8_t numOfHubSetups)
{
    uint8_t newHubAddr = 0;
    uint8_t savedAddrs [255];

    for(int indx = 0; indx < 255; indx++)
    {
        savedAddrs[indx] = 0;
    }

    if(numOfHubSetups <= 0)
    {
        std::cout << "Number of hubs to set up needs to be greater than 0" << std::endl;
    }

    system("clear");
    std::cout << "Begining setup of hub.." << std::endl;

    radioP->begin(); 
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    radioP->setRetries(1, 15);
    radioP->setChannel(124);

    std::cout << "Creating address..." << std::endl;

    //TODO: Get addresses from database (if there are any)
    
    //TODO: Randomly generate an address, add it to the data base
    
    //TODO: Send new address to hub as well as the rest of the addresses
    
    //TODO: Start placment mode (use a function)

    return true;
}


//======================================================================
//Description: Retrieves hub address from database
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
        mysql_close(sqlConnP);
        return false;
    }
    sqlResult = mysql_store_result(sqlConnP);
    sqlRow = mysql_fetch_row(sqlResult);

    while((sqlRow = mysql_fetch_row(sqlResult)) != NULL)
    {
        if(genIndx >= 255)
        {
            std::cout << "\nWARNING: Number of addresses in database exceeds 255!" 
                " Some addresses may be missing!" << std::endl;
            mysql_free_result(sqlResult);
            break;
        }

        hasHubsP[genIndx].address = std::stoi(sqlRow[0]);
        hasHubsP[genIndx].name = sqlRow[1];
        genIndx++;
    }
    return true;
}
