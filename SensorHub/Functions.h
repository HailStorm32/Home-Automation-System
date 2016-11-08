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
//
// Arguments:
//		int errorCode -- integer # between 1-6
//		int address  --  address of receiver (optional)
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
		Serial.print("ERROR 003!! Message not sent to ");
		Serial.print(address);
		Serial.println("!!");
		Serial.println("Please restart!");

		while (true) //Blink error LED
		{
			digitalWrite(ERROR_LED, HIGH);
			delay(700);
			digitalWrite(ERROR_LED, LOW);
			delay(700);
		}
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

}


//========================================================================
// Function: setAddress
// Description: Decode signal from OR gate switch and assign address 
//
// Arguments: 
//		NONE
// Return Values:
//		int -- Station's 4-digit address
//=========================================================================
int setAddress()
{
	//delay(2000);
	bool A = digitalRead(SWITCH_IN_1);
	bool B = digitalRead(SWITCH_IN_2);
	bool C = digitalRead(SWITCH_IN_3);

	if (A == false && B == false && C == false)
	{
		return STATION[0]; //9001
	}

	else if (A == true && B == false && C == false)
	{
		return STATION[1]; //9002
	}

	else if (A == false && B == true && C == false)
	{
		return STATION[2]; //9003
	}

	else if (A == true && B == true && C == false)
	{
		return STATION[3]; //9004
	}

	else if (A == false && B == false && C == true)
	{
		return STATION[4]; //9005
	}

	else if (A == true && B == false && C == true)
	{
		return STATION[5]; //9006
	}

	else if (A == false && B == true && C == true)
	{
		return STATION[6]; //9007
	}

	else if (A == true && B == true && C == true)
	{
		return STATION[7]; //9008
	}
}


//=========================================================================
// Function: giveRange
// Description: Give which address the current hub can read/write to based 
//				on it's address.
//				Each hub has a total of 5 choices to choose from
//
// Arguments:
//		int choice -- what choice to set (# between 1-5)
//		int myaddress -- current address
// Return Value:
//		int -- address for that choice
//=========================================================================
int giveRange(int choice, int myaddress)
{
	switch (choice)
	{
	case 1:
		if (myaddress == STATION[0])
		{
			return myaddress + 1;
		}
		else if(myaddress == STATION[1])
		{
			return myaddress - 1;
		}
		else if (myaddress == STATION[2])
		{
			return myaddress - 2;
		}
		else if (myaddress == STATION[3])
		{
			return myaddress - 3;
		}
		else if (myaddress == STATION[4])
		{
			return myaddress - 1;
		}
		else if (myaddress == STATION[5])
		{
			return myaddress - 1;
		}
		else if (myaddress == STATION[6])
		{
			return myaddress - 1;
		}
		else if (myaddress == STATION[7])
		{
			return myaddress - 1;
		}
		break;
	case 2:
		if (myaddress == STATION[0])
		{
			return myaddress + 2;
		}
		else if (myaddress == STATION[1])
		{
			return myaddress + 1;
		}
		else if (myaddress == STATION[2])
		{
			return myaddress - 1;
		}
		else if (myaddress == STATION[3])
		{
			return myaddress - 2;
		}
		else if (myaddress == STATION[4])
		{
			return myaddress - 2;
		}
		else if (myaddress == STATION[5])
		{
			return myaddress - 2;
		}
		else if (myaddress == STATION[6])
		{
			return myaddress - 2;
		}
		else if (myaddress == STATION[7])
		{
			return myaddress - 2;
		}
		break;
	case 3:
		if (myaddress == STATION[0])
		{
			return myaddress + 3;
		}
		else if (myaddress == STATION[1])
		{
			return myaddress + 2;
		}
		else if (myaddress == STATION[2])
		{
			return myaddress + 1;
		}
		else if (myaddress == STATION[3])
		{
			return myaddress - 1;
		}
		else if (myaddress == STATION[4])
		{
			return myaddress - 3;
		}
		else if (myaddress == STATION[5])
		{
			return myaddress - 3;
		}
		else if (myaddress == STATION[6])
		{
			return myaddress - 3;
		}
		else if (myaddress == STATION[7])
		{
			return myaddress - 3;
		}
		break;
	case 4:
		if (myaddress == STATION[0])
		{
			return myaddress + 4;
		}
		else if (myaddress == STATION[1])
		{
			return myaddress + 3;
		}
		else if (myaddress == STATION[2])
		{
			return myaddress + 2;
		}
		else if (myaddress == STATION[3])
		{
			return myaddress + 1;
		}
		else if (myaddress == STATION[4])
		{
			return myaddress + 1;
		}
		else if (myaddress == STATION[5])
		{
			return myaddress - 4;
		}
		else if (myaddress == STATION[6])
		{
			return myaddress - 4;
		}
		else if (myaddress == STATION[7])
		{
			return myaddress - 4;
		}
		break;
	case 5:
		if (myaddress == STATION[0])
		{
			return myaddress + 5;
		}
		else if (myaddress == STATION[1])
		{
			return myaddress + 4;
		}
		else if (myaddress == STATION[2])
		{
			return myaddress + 3;
		}
		else if (myaddress == STATION[3])
		{
			return myaddress + 2;
		}
		else if (myaddress == STATION[4])
		{
			return myaddress + 2;
		}
		else if (myaddress == STATION[5])
		{
			return myaddress + 1;
		}
		else if (myaddress == STATION[6])
		{
			return myaddress - 5;
		}
		else if (myaddress == STATION[7])
		{
			return myaddress - 5;
		}
		break;
	}
}


//=========================================================================
// Function: startupPings 
// Description: Will ping all hubs in range(defined in giveRange)
//
// Arguments:
//		NONE
// Return Value:
//		True -- pings were successful
//		False -- pings failed
//=========================================================================
bool startupPings()
{

}