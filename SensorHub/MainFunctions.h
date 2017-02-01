//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are licensed by their respective owners		//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//







//=========================================================================
// Function: errorReport
// Description: take an error number and display the proper error 
//				code and LED light.
// Error Codes:
//		01 -- Station address not set
//		02 -- PIR sensor not found
//		03 -- Unable to contact [****] hub
//		04 -- Issue with code
//		05 -- Station [****] didnt send message type
//		06 -- Failed to read from DHT sensor
//		07 -- No acknowledgement ping recieved from [****]
//		08 -- Incorrect acknowledgement ping recieved from [****]
//		09 -- Time out, No ping received
//		10 -- Time out, No data received
//
// Arguments:
//		 errorCode (I) -- integer # between 1-10
//		 address (I) --  address of receiver (optional)
// Return Values:
//		NONE
//==========================================================================
void errorReport(int errorCode, int address = 0) //Function that will take an error number and display the proper error code and LED light
{
	if (errorCode == 1)
	{
		Serial.println("");
		Serial.println("ERROR 001!! Station address not set!");
		Serial.println("Please restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(300);
			digitalWrite(ERROR_LED, LOW);
			delay(300);
		}
	}

	else if (errorCode == 2)
	{
		Serial.println("");
		Serial.println("ERROR 002!! PIR sensor not found!");
		Serial.println("Please check connection and restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(500);
			digitalWrite(ERROR_LED, LOW);
			delay(500);
		}
	}

	else if (errorCode == 3)
	{
		Serial.println(" ");
		Serial.print("ERROR 003!! Unable to contact hub ");
		Serial.print(address);
		Serial.println("!!");
		//Serial.println("Please restart!");

		//while (true) //Blink error LED
	//	{
		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
		//delay(700);
	//}
	}

	else if (errorCode == 4)
	{
		Serial.println(" ");
		Serial.print("ERROR 004!! Issue with code");
		Serial.println("!!");
		Serial.println("Please restart!");

		while (true)
		{
			digitalWrite(ERROR_LED, HIGH);
		}
	}

	else if (errorCode == 5)
	{
		Serial.println(" ");
		Serial.print("ERROR 005!! Station ");
		Serial.print(address);
		Serial.println(" did not set message type!!");
		Serial.println("Please restart!");

		//	while (true)
		//{
		digitalWrite(ERROR_LED, HIGH);
		delay(800);
		digitalWrite(ERROR_LED, LOW);
		delay(800);
		//}
	}

	else if (errorCode == 6)
	{
		Serial.println(" ");
		Serial.print("ERROR 006!! Failed to read from DHT sensor!");
		Serial.println("!!");
		Serial.println("Please restart!");

		while (true)
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(200);
			digitalWrite(ERROR_LED, LOW);
			delay(200);
		}
	}

	else if (errorCode == 7)
	{
		Serial.println(" ");
		Serial.print("ERROR 007!! No acknowledgement ping recieved from ");
		Serial.print(address);
		Serial.println("!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
	}

	else if (errorCode == 8)
	{
		Serial.println(" ");
		Serial.print("ERROR 008!! Incorrect acknowledgement ping recieved from ");
		Serial.print(address);
		Serial.println("!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
	}

	else if (errorCode == 9)
	{
		Serial.println(" ");
		Serial.println("ERROR 009!! Time out! No ping received!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
	}

	else if (errorCode == 10)
	{
		Serial.println(" ");
		Serial.println("ERROR 009!! Time out! No data received!!");

		digitalWrite(ERROR_LED, HIGH);
		delay(1000);
		digitalWrite(ERROR_LED, LOW);
	}

}


//=========================================================================
// Function: receiveAcknowledge
// Description: Receive acknowledgement back from ping
//
// Arguments:
//		acknowledgerAddress -- Address of thehub we want an acknowledge from
// Return Values:
//		True -- recived an acknowledgement that the messsage was recived
//		False -- no acknowledgement was recived
//==========================================================================
bool receiveAcknowledge(int index)
{
	bool didAcknowledge = false;
	int targetTime = 0;
	int data = 0;
	int acknowledgerAddress = RANGE[index];

	radio.openReadingPipe(1, MY_ADDRESS);
	radio.startListening();

	targetTime = millis() + 1000;

	while (!radio.available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 1sec

	if (radio.available())
	{
		radio.read(&data, sizeof(data));

		if (data == acknowledgerAddress)
		{
			Serial.println("Received acknowledge...");//debug only
			didAcknowledge = true;
		}
		else
		{
			errorReport(8, acknowledgerAddress); //Report that we got the wrong data back
			didAcknowledge = false;
		}
	}
	else if (millis() > targetTime)///we went over time, report it
	{
		errorReport(7, acknowledgerAddress);
		didAcknowledge = false;
	}

	return didAcknowledge;
}

//=========================================================================
// Function: sendPings
// Description: Send ping to specified RANGE address 
//
// Arguments:
//		index (I) -- what index of the RANGE array to send to
// Return Value:
//		True -- pings were sent & recived successfully
//		False -- pings failed 
//=========================================================================
bool sendPing(int index)
{
	int toAddress = 0;
	int targetTime = 0;

	Serial.println(" ");//debug only
	Serial.println("Sending pings...");//debug only

	radio.stopListening();
	radio.openWritingPipe(RANGE[index]);

	//\\\\\\\\\\\\\\\\\\\\\\\\\ Send "To Address" //////////////////////////

	toAddress = RANGE[index];

	if (!radio.write(&toAddress, sizeof(toAddress)))
	{
		errorReport(3, toAddress);
		return false;
	}

	delay(200);

	//\\\\\\\\\\\\\\\\\\\\\\\\ Send "from address" ////////////////////////
	if (!radio.write(&MY_ADDRESS, sizeof(MY_ADDRESS)))
	{
		errorReport(3, toAddress);
		return false;
	}


	return true;
}


//=========================================================================
// Function: receivePing
// Description: Receive ping from master hub and send back 
//				an acknowledgement ping 
//
// Arguments:
//		NONE
// Return Value:
// Return Value:
//		True -- pings recived successfully
//		False -- pings not received 
//=========================================================================
bool receivePing()
{
	bool pingReceive = false;
	int targetTime = 0;
	int toAddress = 0;
	int fromAddress = 0;

	radio.openReadingPipe(1, MY_ADDRESS);
	radio.startListening();


	do
	{
		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Receive the "toAddress" //////////////////////////
		targetTime = millis() + 30000;
		while (!radio.available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 30sec

		if (radio.available())
		{
			Serial.println("Receiving Data..."); //debug only
			radio.read(&toAddress, sizeof(toAddress));
		}
		else if (millis() > targetTime)
		{
			errorReport(9);
		}

		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Receive the "fromAddress" //////////////////////////
		targetTime = millis() + 30000;
		while (!radio.available() && millis() < targetTime) {} //Wait until there is a message to read, but dont wait longer than 30sec

		if (radio.available())
		{
			radio.read(&fromAddress, sizeof(fromAddress));
		}
		else if (millis() > targetTime)
		{
			errorReport(9);
		}

		//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ Make Sure we received the right address ///////////////////
		if (toAddress == MY_ADDRESS)
		{
			radio.stopListening();
			radio.openWritingPipe(RANGE[0]);

			Serial.println("Sending Ping back..."); //debug only

			if (radio.write(&MY_ADDRESS, sizeof(MY_ADDRESS)))
			{
				pingReceive = true;
			}
			else
			{
				errorReport(3, RANGE[0]);
			}
		}

	} while (toAddress != MY_ADDRESS); //Loop until we receive the address that we want

	return pingReceive;
}


//=========================================================================
// Function: startupPings 
// Description: Will send or receive pings based on hub's address
//				Will only do so for up to address 1004
//
// Arguments:
//		NONE
// Return Value:
//		True -- pings were successful
//		False -- pings failed
//=========================================================================
bool startupPings()
{
	bool pingsSuccess = false;
	int debug = 0; //for debug only

	if (MY_ADDRESS == 9001)
	{

		for (int i = 0; i < 5; i++)
		{

			debug = RANGE[i];//debug only

			if (sendPing(i) == true && receiveAcknowledge(i) == true)
			{
				Serial.println("Successful contact with hub ");//debug only
				Serial.println(debug);//debug only
			}
		}

	}
	else
	{
		Serial.println("Waiting for pings...");//debug only
		pingsSuccess = receivePing();
	}

	return pingsSuccess;
}


//=========================================================================
// Function: waitForData
// Description: Wait until there is data to read
//				Wait no longer than specified time
//
// Arguments:
//		waitTime -- time to wait (in msec) before calling it
//					value < 0 -- infinite wait time
// Return Values:
//		True -- there was data to read with in the specified time
//		False -- there was NO data to read with in the specified time
//=========================================================================
bool waitForData(int waitTime)
{
	int targetTime = 0;

	if (waitTime >= 0)
	{
		targetTime = millis() + waitTime;
		while (!radio.available() && millis() < targetTime) {}

		if (millis() > targetTime)
		{
			errorReport(10);
		}
	}
	else
	{
		while (!radio.available() && millis() < targetTime) {}
	}

}

//=========================================================================
// Function: logTemperature
// Description: Put temperature data from a hub into its specific array
//
// Arguments:
//		Data (I) -- unsorted temperature data
//		hubTemperature[] (O) -- array of each hub's temperature data
//		fromAddress (I) -- address from which the data was received
// Return Values: 
//		NONE
//=========================================================================
void logTemperature(int data, int hubTemperature[], int fromAddress)
{
	switch (fromAddress)
	{
	case 9001:
		hubTemperature[0] = data;
		break;
	case 9002:
		hubTemperature[1] = data;
		break;
	case 9003:
		hubTemperature[2] = data;
		break;
	case 9004:
		hubTemperature[3] = data;
		break;
	case 9005:
		hubTemperature[4] = data;
		break;
	case 9006:
		hubTemperature[5] = data;
		break;
	case 9007:
		hubTemperature[6] = data;
		break;
	case 9008:
		hubTemperature[7] = data;
		break;
	}
}


//=========================================================================
// Function: logTemperature
// Description: Put temperature data from a hub into its specific array
//
// Arguments:
//		Data (I) -- unsorted temperature data
//		hubMotion[] (O) -- array of each hub's motion data
//		fromAddress (I) -- address from which the data was received
// Return Values: 
//		NONE
//=========================================================================
void logMotion(int data, int hubMotion[], int fromAddress)
{
	switch (fromAddress)
	{
	case 9001:
		hubMotion[0] = data;
		break;
	case 9002:
		hubMotion[1] = data;
		break;
	case 9003:
		hubMotion[2] = data;
		break;
	case 9004:
		hubMotion[3] = data;
		break;
	case 9005:
		hubMotion[4] = data;
		break;
	case 9006:
		hubMotion[5] = data;
		break;
	case 9007:
		hubMotion[6] = data;
		break;
	case 9008:
		hubMotion[7] = data;
		break;
	}
}

//=========================================================================
// Function: dataSend
// Description: Will send data to specified hub
//
// Arguments:
//		toAddress (I) -- what address we want to send to
//		data (I) -- data to send
// Return Values:
//		NONE
//=========================================================================
void dataSend(int data, int toAddress = 0)
{
	digitalWrite(DEBUG_LED, HIGH);

	radio.openWritingPipe(9001);
	radio.stopListening();

	if (radio.write(&data, sizeof(data)))
	{
		Serial.println("true");
	}
	else
	{
		//Serial.println("Sending to address");
		errorReport(3, 9001);
	}

	digitalWrite(DEBUG_LED, LOW);
}


//=========================================================================
// Function: receiveData
// Description: Will receive data from hub
//
// Arguments:
//		data (O) -- holds incoming data
// Return Values:
//		address -- target address
//=========================================================================
int dataReceive(int &data)
{
	int address = 0;

	radio.openReadingPipe(0, MY_ADDRESS); //must be set to receiver's address. Will only receive data from hubs that have opened writing pipes to this address
	radio.startListening();

	waitForData(5000);

	radio.read(&data, sizeof(data));

	//address

	return 1;
}