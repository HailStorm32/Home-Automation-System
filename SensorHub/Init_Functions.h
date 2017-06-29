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



/*
Self Note:
Each hub can only write to 1x 'ToAddress' (the hub its sending it to) at a time, but can recive from multiple different 'FromAddress' (the hub's address) at a time.
*/

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
	Serial.begin(9600);
	//delay(2000);
	bool A = digitalRead(SWITCH_IN_1);
	bool B = digitalRead(SWITCH_IN_2);
	bool C = digitalRead(SWITCH_IN_3);
	
	Serial.println("01");//Debug only
	
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
//		 choice (I) -- what choice to set (# between 1-5)
//		 MY_ADDRESS (I) -- current address
// Return Value:
//		int -- address for that choice
//=========================================================================
int giveRange(int choice, int MY_ADDRESS)
{
	Serial.println("02");
	switch (choice)
	{
	case 1:
		if (MY_ADDRESS == STATION[0])
		{
			return MY_ADDRESS + 1;
		}
		else if (MY_ADDRESS == STATION[1])
		{
			return MY_ADDRESS - 1;
		}
		else if (MY_ADDRESS == STATION[2])
		{
			return MY_ADDRESS - 2;
		}
		else if (MY_ADDRESS == STATION[3])
		{
			return MY_ADDRESS - 3;
		}
		else if (MY_ADDRESS == STATION[4])
		{
			return MY_ADDRESS - 1;
		}
		else if (MY_ADDRESS == STATION[5])
		{
			return MY_ADDRESS - 1;
		}
		else if (MY_ADDRESS == STATION[6])
		{
			return MY_ADDRESS - 1;
		}
		else if (MY_ADDRESS == STATION[7])
		{
			return MY_ADDRESS - 1;
		}
		break;
	case 2:
		if (MY_ADDRESS == STATION[0])
		{
			return MY_ADDRESS + 2;
		}
		else if (MY_ADDRESS == STATION[1])
		{
			return MY_ADDRESS + 1;
		}
		else if (MY_ADDRESS == STATION[2])
		{
			return MY_ADDRESS - 1;
		}
		else if (MY_ADDRESS == STATION[3])
		{
			return MY_ADDRESS - 2;
		}
		else if (MY_ADDRESS == STATION[4])
		{
			return MY_ADDRESS - 2;
		}
		else if (MY_ADDRESS == STATION[5])
		{
			return MY_ADDRESS - 2;
		}
		else if (MY_ADDRESS == STATION[6])
		{
			return MY_ADDRESS - 2;
		}
		else if (MY_ADDRESS == STATION[7])
		{
			return MY_ADDRESS - 2;
		}
		break;
	case 3:
		if (MY_ADDRESS == STATION[0])
		{
			return MY_ADDRESS + 3;
		}
		else if (MY_ADDRESS == STATION[1])
		{
			return MY_ADDRESS + 2;
		}
		else if (MY_ADDRESS == STATION[2])
		{
			return MY_ADDRESS + 1;
		}
		else if (MY_ADDRESS == STATION[3])
		{
			return MY_ADDRESS - 1;
		}
		else if (MY_ADDRESS == STATION[4])
		{
			return MY_ADDRESS - 3;
		}
		else if (MY_ADDRESS == STATION[5])
		{
			return MY_ADDRESS - 3;
		}
		else if (MY_ADDRESS == STATION[6])
		{
			return MY_ADDRESS - 3;
		}
		else if (MY_ADDRESS == STATION[7])
		{
			return MY_ADDRESS - 3;
		}
		break;
	case 4:
		if (MY_ADDRESS == STATION[0])
		{
			return MY_ADDRESS + 4;
		}
		else if (MY_ADDRESS == STATION[1])
		{
			return MY_ADDRESS + 3;
		}
		else if (MY_ADDRESS == STATION[2])
		{
			return MY_ADDRESS + 2;
		}
		else if (MY_ADDRESS == STATION[3])
		{
			return MY_ADDRESS + 1;
		}
		else if (MY_ADDRESS == STATION[4])
		{
			return MY_ADDRESS + 1;
		}
		else if (MY_ADDRESS == STATION[5])
		{
			return MY_ADDRESS - 4;
		}
		else if (MY_ADDRESS == STATION[6])
		{
			return MY_ADDRESS - 4;
		}
		else if (MY_ADDRESS == STATION[7])
		{
			return MY_ADDRESS - 4;
		}
		break;
	case 5:
		if (MY_ADDRESS == STATION[0])
		{
			return MY_ADDRESS + 5;
		}
		else if (MY_ADDRESS == STATION[1])
		{
			return MY_ADDRESS + 4;
		}
		else if (MY_ADDRESS == STATION[2])
		{
			return MY_ADDRESS + 3;
		}
		else if (MY_ADDRESS == STATION[3])
		{
			return MY_ADDRESS + 2;
		}
		else if (MY_ADDRESS == STATION[4])
		{
			return MY_ADDRESS + 2;
		}
		else if (MY_ADDRESS == STATION[5])
		{
			return MY_ADDRESS + 1;
		}
		else if (MY_ADDRESS == STATION[6])
		{
			return MY_ADDRESS - 5;
		}
		else if (MY_ADDRESS == STATION[7])
		{
			return MY_ADDRESS - 5;
		}
		break;
	}
}