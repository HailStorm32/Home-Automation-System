bool hubSetup(RF24 *radioP, uint8_t numOfHubSetups = 1)


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
bool hubSetup(RF24 *radioP, uint8_t numOfHubSetups = 1)
{
    uint8_t newHubAddr = 0;
    uint8_t savedAddrs [255];

    for(int indx = 0; indx < 255; indx++)
    {
        savedAddrs[indx] = 0;
    }

    system("clear");
    std::cout << "Begining setup of hub.." << std::endl;

    radioP->begin(); 
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    radioP->setRetries(1, 15);
    radio->setChannel(124);

    std::cout << "Creating address..." << std::endl;

    //TODO: Get addresses from database (if there are any)
    
    //TODO: Randomly generate an address, add it to the data base
    
    //TODO: Send new address to hub as well as the rest of the addresses
    
    //TODO: Start placment mode (use a function)
}
