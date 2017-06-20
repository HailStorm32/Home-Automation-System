//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are licensed by their respective owners		//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//
#pragma once
#include "Arduino.h"

const int STATION[8] = { 9001, 9002, 9003, 9004, 9005, 9006, 9007, 9008 }; //Set the addresses for all stations

//Set Arduino pins
const byte SWITCH_IN_1 = 2;
const byte SWITCH_IN_2 = 3;
const byte SWITCH_IN_3 = 4;
const byte ERROR_LED = 7;
const byte PIR = 5;
const byte DEBUG_LED = 8;

//Timer consts
const int SEND_TIME = 10000; //Time it takes a hub to completly send a message
const int BASE_CYCLE_TIME = 6000; //Time between data transmissions (in milliseconds) //Base time will be multiplied by each stations number so each hub will send data at different times 
const int READ_CYCLE = 3500;  //Time between reading the temp sensor (in milliseconds)  1000 = 1sec


const unsigned int MESSAGE_SIZE = 17;
const unsigned int RANGE_SIZE = 5;
