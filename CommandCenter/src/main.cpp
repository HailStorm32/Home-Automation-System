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

#include "functions.h"

const uint8_t STATUS_LED = 1;
const uint8_t ERROR_LED = 4;
const uint8_t NUM_OF_MENU_ITEMS = 2;
const uint8_t VALID_MENU_ITEMS[] = {1,2};

int main()
{
    //Setup
    wiringPiSetup();

    AES256 test;
    RF24 radio(3, 7);
    radio.begin();
   
    pinMode(STATUS_LED, OUTPUT);


    //Code
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(ERROR_LED, LOW);
   
    //TODO: Check database for server address. If already generated, take it.
    //If not generated, generate it and then save it to database

    bool exitFlag = false;
    int menuItemNum;
    string userEntry = "";

    //Clear screen
    system("clear");
   
    do
    {
        userEntry = "";

        std::cout << "Welcome to the Home Automation System Server Interface!" << std::endl;
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
