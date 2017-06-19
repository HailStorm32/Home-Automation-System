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
	Hub();

	//--------- Modifiers -------------//
	
	bool logTemperature(float data);

	bool logMotion(int data);

	//---------- Accessors -----------//
	float getTemerature() const;

	float getMotion() const;

	//--------- Other --------------//
	void errorReport(int errorCode, int address = 0);

private:
	int temperature;
	int motion;
};