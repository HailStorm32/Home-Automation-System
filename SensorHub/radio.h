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
#include "hub.h"

class Radio
{
public:
	//--------- Constructor --------//
	Radio(const int myAddress, const int range[], RF24* radioPointer, Hub* systemPointer);

	//--------- Other ----------//
	/*
	Input:
		temperature -- 0.0-999.9
		motion -- 0-9999
		fromAddress -- 9001-9008
		toAddress - 9001-9008
	Returns:
		False -- if failed
		True -- if succeeded
	*/
	bool sendData(float temperature, int motion, int fromAddress, int toAddress);

	/*
	Input:
		NONE
	Returns:
		False -- if failed
		True -- if succeeded
	*/
	bool receiveData();

	/*
	Input:
		toAddress -- The address of the hub we want data from
	Returns:
		False -- if failed
		True -- if succeeded
	*/
	bool requestData(int toAddress);

	/*
	Description:
		Will wait for a request from the master hub
	Return Value:
		True -- received a request for data
		False -- waiting for request
	*/
	bool waitForRequest();

	/*
	Description:
		Will send or receive pings based on hub's address
		Will only do so for up to address 1004
	Return Value:
		True -- pings were successful
		False -- pings failed
	*/
	bool startupPings();

	/*
	Input:
		temperature -- 0.0-999.9
		motion -- 0-9999
		fromAddress -- 9001-9008
	Returns:
		NONE
	*/
	String encodeMessage(float temperature, int motion, int fromAddress, int toAddress);

private:
	char message[MESSAGE_SIZE];

	int fromAddress;
	RF24 * radioP;
	Hub * systemP;
	int range[RANGE_SIZE];
	int myAddress;
	unsigned int numOfRetries;


	/*
	Input:
		codedMessage (I) -- string of coded message
		temperature (O)
		motion (O)
		fromAddress (O)
	Returns:
		True -- decoded successfully
		False -- NOT decoded successfully
	*/
	bool decodeMessage(float &temperature, int &motion, int &fromAddress, int &toAddress, const String &codedMessage);

	/*
	Input:
		waitTime -- integer value
					value < 0 -- infinite wait time
	Returns:
		True -- there was data to read with in the specified time
		False -- there was NO data to read with in the specified time
	*/
	bool waitForData(int milliSec, bool printError = true);

	/*
	Input:
		codedMessage (I) -- string of coded message
	Returns:
		True -- message was formated correctly
		False -- message was NOT formated correctly
	*/
	bool isValidMessage(const String &codedMessage);

	/*
	Description: 
		Receive ping from master hub and send back 
		an acknowledgement ping 
   Return Value:
  		True -- pings recived successfully
  		False -- pings not received 
	*/
	bool receivePing();

	/*
	Description:
		Send ping to specified RANGE address 
	Arguments:
  		index (I) -- what index of the RANGE array to send to
	Return Value:
  		True -- pings were sent & recived successfully
  		False -- pings failed 
	*/
	bool sendPing(int index);

	/*
	Description: 
		Receive acknowledgement back from ping
	Arguments:
  		acknowledgerAddress -- Address of the hub we want an acknowledge from
	Return Values:
  		True -- recived an acknowledgement that the messsage was recived
  		False -- no acknowledgement was recived
	*/
	bool receiveAcknowledge(int index);
};
