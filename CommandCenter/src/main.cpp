#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <time.h>
#include <chrono>
#include <queue>

#include <Crypto.h>
#include <wiringPi.h>
#include <RF24.h>
#include <AES.h>
#include <mysql/mysql.h>
#include <config.h>



const uint8_t STATUS_LED = 1;
const uint8_t ERROR_LED = 4;
const uint8_t NUM_OF_MENU_ITEMS = 4;
const uint8_t VALID_MENU_ITEMS[] = {1,2,3,4};
const bool QUERY_ERROR_CHECK = true;
const uint8_t MAX_NUM_OF_ADDRS = 32;
const uint8_t COMMAND_SIZE = 3; 


struct node
{
    byte adjNodes[MAX_NUM_OF_ADDRS];
    byte parentNode;
};

node addrGraph[MAX_NUM_OF_ADDRS];

#include "functions.h"

int main()
{
    bool exitFlag = false;
    int menuItemNum;
    int genIndx = 0;
    int numOfSetupHubs;
    string userEntry = "";
    string sqlQuery;
    bool haveAddrs;
    uint8_t hubAddrs[255] = {};
    uint8_t myAddress = 0;
    hub hasHubs[255];

    MYSQL *sqlConn;
    MYSQL_RES *sqlResult;
    MYSQL_ROW sqlRow;

    //Setup
    wiringPiSetup();

    AES256 test;
    //Pin setup: http://tmrh20.github.io/RF24/index.html
    RF24 radio(22, 0);
    radio.begin();
   
    pinMode(STATUS_LED, OUTPUT);


    //Code
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(ERROR_LED, LOW);

    //Init the hub address array
    for(int indx = 0; indx < 255; indx++)
    {
        hubAddrs[indx] = 0;
    }

    for(int indx = 0; indx < MAX_NUM_OF_ADDRS; indx++)
    {
        hasHubs[indx].lastUsedId = 0;
    }
    
    if(!QUERY_ERROR_CHECK)
    {
        std::cout << "\nWARNING: Error checking for database queries is off!"
            << std::endl;
    }


    //Make connection to database
    sqlConn = mysql_init(sqlConn);
    mysql_options(sqlConn, MYSQL_READ_DEFAULT_GROUP, "CommandCenter");
    if(!mysql_real_connect(sqlConn, DB_IP.c_str(), DB_USER.c_str(),
            DB_PASS.c_str(), DB_NAME.c_str(), 8080, NULL, 0))
    {
        std::cout << "\nERROR: Cant connect to SQL database!" << std::endl;
        std::cout << mysql_error(sqlConn) << endl;
        mysql_close(sqlConn);
        return 1;
    }

    //Check to see if we have setup any hubs
    //and get the addresses if we have them
    if(!getNumOfSetupHubs(sqlConn, &numOfSetupHubs))
    {
        return 1;
    }

    if(numOfSetupHubs > 0)
    {
        haveAddrs = true;
        
        if(!retrieveAddrs(sqlConn, hasHubs))
        {
            return 1;
        }
        
        myAddress = hasHubs[0].address;
    }
    else
    {
        haveAddrs = false;
        
        //TODO: Generate random number using library for server address

        do
        {
            srand(time(NULL));
            myAddress = rand() % 255;
        }while(myAddress == 0);

        //Add the server address to the database
        sqlQuery = "INSERT INTO hubs VALUES('" + std::to_string(myAddress) +
            "','server')";
        if(mysql_query(sqlConn, sqlQuery.c_str()) && QUERY_ERROR_CHECK)
        {
            std::cout << "\nERROR: sql query failed to update hub addresses!"
                << std::endl;
            std::cout << mysql_error(sqlConn) << endl;
            mysql_close(sqlConn);
            return 1;
        }
        
        //Update the databases count of how many hubs have been setup
        sqlQuery = "UPDATE variables SET value='1' WHERE name='numOfSetupHubs'";
        if(mysql_query(sqlConn, sqlQuery.c_str()) && QUERY_ERROR_CHECK)
        {
            std::cout << "\nERROR: sql query failed to update variable!"
                << std::endl;
            std::cout << mysql_error(sqlConn) << endl;
            mysql_close(sqlConn);
            return 1;
        }
    }



    //Clear screen
    system("clear");
   
    do
    {
        userEntry = "";

        std::cout << "Welcome to the H.A.S. Server Interface!" 
            << std::endl;
        std::cout << "Menu:" << std::endl;
        std::cout << "1). Setup new hub \n"
            "2). Test Setup \n"
            "3). Remove Tests \n"
            "4). Exit" << std::endl;
        std::cout << "\nChoice:";
        std::cin >> userEntry; 
       
        //Validate entry
        for(int indx = 0; indx < NUM_OF_MENU_ITEMS; indx++)
        {
            if(userEntry == std::to_string(VALID_MENU_ITEMS[indx]))
            {
                menuItemNum = std::stoi(userEntry);
                break;
            }
            else
            {
                menuItemNum = -1;
            }
        }

        switch(menuItemNum)
        {
            //TODO: Add a fuction to rename a hub
            case 1:
                if(fullHubSetup(&radio, hasHubs, sqlConn) == 1)
                {
                    return 1;
                }
                
                

                break;
            case 2:
                if(testHubSetup(&radio, hasHubs, sqlConn) == 1)
                {
                    return 1;
                }
                break;
            case 3:
                if(removeTestHubs(sqlConn))
                {
                    std::cout << "\n\nTest hubs removed..." << std::endl;
                }
                break;
            case 4:
                exitFlag = true;
                break;
            default:
                system("clear");
                std::cout << "\nInvalid menu option: " << userEntry << "\n\n";
                break;
        }

    }while(!exitFlag);
}
