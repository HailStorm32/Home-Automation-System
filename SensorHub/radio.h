#pragma once

const unsigned int MESSAGE_SIZE = 17;

class Radio
{
public:
	//--------- Constructor --------//
	Radio(unsigned int myAddress, unsigned int errorLed);

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
	temperature -- 0.0-999.9
	motion -- 0-9999
	fromAddress -- 9001-9008
	Returns:
	NONE
	*/
	String encodeMessage(float temperature, int motion, int fromAddress);

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
	bool decodeMessage(float &temperature, int &motion, int &fromAddress, const String &codedMessage);//char codedMessagefloat[], float &temperature, int &motion, int &fromAddress);



private:
	char message[MESSAGE_SIZE];

	//int temperature;
	//int motion;
	int fromAddress;
	//int toAddress;
	int errorLed;

	/*
	Input:
		temperature -- 0.0-999.9
		motion -- 0-9999
		fromAddress -- 9001-9008
	Returns:
		NONE
	*/
	//void encodeMessage(float temperature, int motion, int fromAddress, char codedMessage[]);

	/*
	Input:
		codedMessage (I) -- cstring of coded message
		temperature (O)
		motion (O)
		fromAddress (O)
	Returns:
		True -- decoded successfully
		False -- NOT decoded successfully
	*/
	//bool decodeMessage(char codedMessagefloat[], float &temperature, int &motion, int &fromAddress);

	/*
	Input:
		waitTime -- integer value
					value < 0 -- infinite wait time
	Returns:
		True -- there was data to read with in the specified time
		False -- there was NO data to read with in the specified time
	*/
	bool waitForData(int waitTime);

	/*
	Input:
		codedMessage (I) -- string of coded message
	Returns:
		True -- message was formated correctly
		False -- message was NOT formated correctly
	*/
	bool isValidMessage(const String &codedMessage);
};
