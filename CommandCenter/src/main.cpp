#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <thread>
#include <chrono>

#include <Crypto.h>
#include <wiringPi.h>
#include <RF24.h>
#include <AES.h>
#include <mysql/mysql.h>
#include <config.h>


const uint8_t STATUS_LED = 1;
const uint8_t ERROR_LED = 4;
const uint8_t NUM_OF_MENU_ITEMS = 2;
const uint8_t VALID_MENU_ITEMS[] = {1,2};
const bool QUERY_ERROR_CHECK = true;

#include "functions.h"

int main()
{
    bool exitFlag = false;
    int menuItemNum;
    int genIndx = 0;
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
    RF24 radio(3, 7);
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
        mysql_close(sqlConn);
        return 1;
    }

    //Check to see if we have setup any hubs
    sqlQuery = "SELECT * FROM variables WHERE name='numOfSetupHubs'";
    if(mysql_query(sqlConn, sqlQuery.c_str()) 
            && QUERY_ERROR_CHECK)
    {
        std::cout << "\nERROR: sql query failed to get 'numOfSetupHubs'!" 
            << std::endl;
        mysql_close(sqlConn);
        return 1;
    }
    sqlResult = mysql_store_result(sqlConn);
    sqlRow = mysql_fetch_row(sqlResult);
    
    //Get the addresses if we have them
    if(std::stoi(sqlRow[1]) > 0)
    {
        haveAddrs = true;
        mysql_free_result(sqlResult);
        
        retrieveAddrs(sqlConn, hasHubs);
        
        myAddress = hasHubs[0].address;
    }
    else
    {
        haveAddrs = false;
        mysql_free_result(sqlResult);
        
        //TODO: Generate random number using library for server address
        
        myAddress = 42; //Place holder value
        sqlQuery = "INSERT INTO hubs VALUES('" + std::to_string(myAddress) +
            "','server')";

        if(mysql_query(sqlConn, sqlQuery.c_str()) && QUERY_ERROR_CHECK)
        {
            std::cout << "\nERROR: sql query failed to update hub addresses!"
                << std::endl;
            std::cout << "DEBUG: Continuing anyway... Data may be missing" 
                << endl;
            mysql_close(sqlConn);
            return 1;
        }
        
        sqlQuery = "UPDATE variables SET value='1' WHERE name='numOfSetupHubs'";
        
        if(mysql_query(sqlConn, sqlQuery.c_str()) && QUERY_ERROR_CHECK)
        {
            std::cout << "\nERROR: sql query failed to update variable!"
                << std::endl;
            std::cout << mysql_info(sqlConn) << std::endl;
            std::cout << mysql_error(sqlConn) << std::endl;
            std::cout << mysql_errno(sqlConn) << std::endl;
            mysql_close(sqlConn);
            return 1;
        }
    }



    //Clear screen
    system("clear");
   
    do
    {
        userEntry = "";

        std::cout << "Welcome to the Home Automation System Server Interface!" 
            << std::endl;
        std::cout << "Menu:" << std::endl;
        std::cout << "1). Setup new hub \n"
            "2). Exit" << std::endl;
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
            case 1:
                //TODO: Run hub setup function
                system("clear");
                break;
            case 2:
                exitFlag = true;
                break;
            default:
                system("clear");
                std::cout << "\nInvalid menu option: " << userEntry << "\n\n";
                break;
        }

    }while(!exitFlag);
}
