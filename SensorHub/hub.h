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
#include "constants.h"
#include "Arduino.h"

class Hub
{
public:
	//--------- Constructor ------------//
	/*
	Input:
		debugMode (I) -- Set to true to print all debug messages and false to NOT
	*/
	Hub(bool debugMode);

	//--------- Modifiers -------------//
	
	bool logTemperature(float data);

	bool logMotion(int data);

	//---------- Accessors -----------//
	float getTemerature() const;

	float getMotion() const;

	
	//--------- Other --------------//
	
	/*
	Description:
		Gives out error messages based on the inputed error code
	Input:
		errorCode (I) -- integer # between 1-10
		address (I) --  address of receiver (optional)
	*/
	void errorReport(int errorCode, int address = 0);

	/*
	Description:
		Prints given message via systemP->debugPrint, only if debugMode flag is set to true
	Input:
		message (I) -- string to print
	*/
	void debugPrint(String message);

	/*
	Description:
		Prints given message via systemP->debugPrint, only if debugMode flag is set to true
	Input:
		data (I) -- float data to print
	*/
	void debugPrint(float data);

private:
	int temperature;
	int motion;
	bool debugMode;
};