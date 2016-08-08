//**************************************************************************************//
//																						//
//			This code by Demetrius Van Sickle is licensed under a						//
//		Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																						//
//	The nRF24L01 libraries used in this code are copyrighted by their respective owners	//
//			and are issued under the GNU General Public License version 2.				//
//																					    //
//**************************************************************************************//


//SetReadPipes
void SetReadRange()
{

	if (MyAddress == Station[0])
	{
		Range[0] = MyAddress + 1;
		Range[1] = MyAddress + 2;
		Range[2] = MyAddress + 3;
		Range[3] = MyAddress + 4;
		Range[4] = MyAddress + 5;
		//Range[5] = MyAddress + 6;
	}

	else if (MyAddress == Station[1])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress + 1;
		Range[2] = MyAddress + 2;
		Range[3] = MyAddress + 3;
		Range[4] = MyAddress + 4;
		//Range[5] = MyAddress + 5;
	}

	else if (MyAddress == Station[2])
	{
		Range[0] = MyAddress - 2;
		Range[1] = MyAddress - 1;
		Range[2] = MyAddress + 1;
		Range[3] = MyAddress + 2;
		Range[4] = MyAddress + 3;
		//Range[5] = MyAddress + 4;
	}

	else if (MyAddress == Station[3])
	{
		Range[0] = MyAddress - 3;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 1;
		Range[3] = MyAddress + 1;
		Range[4] = MyAddress + 2;
		//Range[5] = MyAddress + 3;
	}

	else if (MyAddress == Station[4])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress + 1;
		Range[4] = MyAddress + 2;
		//Range[5] = MyAddress + 3;
	}

	else if (MyAddress == Station[5])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress - 4;
		Range[4] = MyAddress + 1;
		//Range[5] = MyAddress + 2;
	}

	else if (MyAddress == Station[6])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress - 4;
		Range[4] = MyAddress - 5;
		//Range[5] = MyAddress + 1;
	}

	else if (MyAddress == Station[7])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress - 4;
		Range[4] = MyAddress - 5;
		//Range[5] = MyAddress - 6;
	}

}


void SetAddress() //Decode signal from OR gate switch and assign address 
{
	
	delay(2000);

	bool A = digitalRead(switchIn_1);
	bool B = digitalRead(switchIn_2);
	bool C = digitalRead(switchIn_3);

	if (A == false && B == false && C == false)
	{
		MyAddress = Station[0]; //9001
	}

	else if (A == true && B == false && C == false)
	{
		MyAddress = Station[1]; //9002
	}

	else if (A == false && B == true && C == false)
	{
		MyAddress = Station[2]; //9003
	}

	else if (A == true && B == true && C == false)
	{
		MyAddress = Station[3]; //9004
	}

	else if (A == false && B == false && C == true)
	{
		MyAddress = Station[4]; //9005
	}

	else if (A == true && B == false && C == true)
	{
		MyAddress = Station[5]; //9006
	}

	else if (A == false && B == true && C == true)
	{
		MyAddress = Station[6]; //9007
	}

	else if (A == true && B == true && C == true)
	{
		MyAddress = Station[7]; //9008
	}
}


void Error(int errorCode, int address = 0) //Function that will take an error number and display the proper error code and LED light
{
	if (errorCode == 1)
	{
		Serial.println("");
		Serial.println("ERROR 001!! Station address not set!");
		Serial.println("Please restart!");

		while (true) //Blink error LED
		{
			digitalWrite(errorLED, HIGH);
			delay(300);
			digitalWrite(errorLED, LOW);
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
			digitalWrite(errorLED, HIGH);
			delay(500);
			digitalWrite(errorLED, LOW);
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
			digitalWrite(errorLED, HIGH);
			delay(700);
			digitalWrite(errorLED, LOW);
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
			digitalWrite(errorLED, HIGH);

		}
	}

	else if (errorCode == 5)
	{
		Serial.println(" ");
		Serial.print("ERROR 005!! Station ");
		Serial.print(address);
		Serial.println(" did not set message type!!");
		Serial.println("Please restart!");

		while (true)
		{
			digitalWrite(errorLED, HIGH);
			delay(800);
			digitalWrite(errorLED, LOW);
			delay(800);
		}
	}

}


/*void SendText(char message[] = {0}, int address = 0)
{
	radio.openWritingPipe(MyAddress);
	radio.stopListening();

	unsigned long time = millis();
	time = time + 5000;

	while (radio.write(&message, sizeof(message)) != true)
	{	
		if (millis() > time)
		{
			Error(3, address);
		} 
	}	
}*/


void SendNum(int AmountofData, String data1 = "       ", String data2 = "       ", String data3 = "       ", int TOaddress = 0)
{
	radio.openWritingPipe(MyAddress);
	radio.stopListening();

	int data[3] = {}; //Have to create 4 arrays otherwise it will cause a weird bug...

	String Type = "  ";

	int nul = 999;
	int Number = 0;

	Type[0] = data1[6];
	Type[1] = data2[6];
	Type[3] = data3[6];

	data[0] = data1.toInt();
	data[1] = data2.toInt();
	data[2] = data3.toInt();


	Serial.println("/////////////");
	Serial.println(TOaddress);
	Serial.println(data1);
	Serial.println(Type[0]);
	Serial.println(data[0]);


	///////////Send the address it wants to send to/////
	unsigned long time = millis();
	time = time + 5000;

	while (radio.write(&TOaddress, sizeof(TOaddress)) != true)
	{
		if (millis() > time)
		{
			Error(3, TOaddress);
		}
	}

	///////////Send my address ////////////////////////////
	time = millis();
	time = time + 5000;

	while (radio.write(&MyAddress, sizeof(MyAddress)) != true)
	{
		if (millis() > time)
		{
			Error(3, TOaddress);
		}
	}

	///////////Tell the receiver how much data we are sending ////////////////////////////
	time = millis();
	time = time + 5000;

	while (radio.write(&AmountofData, sizeof(AmountofData)) != true)
	{
		if (millis() > time)
		{
			Error(3, TOaddress);
		}
	}



	/////////Start sending the data////////////
	
	for(int i = 0; i != AmountofData; i++)
	{
		//////////////Tell the receiver what type of message it is (Motion, Temperature or Random)////////////
		time = millis();
		time = time + 5000;

		while (radio.write(&Type[i], sizeof(Type[i])) != true)
		{
			if (millis() > time)
			{
				Error(3, TOaddress);
			}
		}

		///////////////////Send the number data////////////////////////////
		time = millis();
		time = time + 5000;

		while (radio.write(&data[i], sizeof(data[i])) != true)
		{
			if (millis() > time)
			{
				Error(3, TOaddress);
			}
		}
	}
	
	Serial.println(" ");
	Serial.print("Message(s) sent to ");
	Serial.print(TOaddress);

} 


void FirstPing()
{
	char messageS[] = "ping";
	char messageR[5] = {0};
	int time = 0;
	bool flag = false;

	
	switch (MyAddress)
	{
	case 9001:
		Serial.println(" ");
		Serial.println("case 9001 start");
		for (int i = 0; i != 1; i++)// 'i' needs to be set to 1 more than the max # of values// Default # is 6
		{
			flag = false;

			radio.openWritingPipe(MyAddress);
			radio.stopListening();
			
			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Range[i]); //1
			Serial.print(" ...");

			time = millis();
			time = time + 1000;
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3, Range[i]);
				}
			}

			Serial.println(" ");
			Serial.print("Message sent to ");
			Serial.print(Range[i]); //1 



			radio.openReadingPipe(0, Station[1]); //Start listening for station2
			radio.startListening();

			Serial.println(" ");
			Serial.print("Receiving from ");
			Serial.print(Range[i]); //1
			Serial.print(" ...");

			while (flag == false)
			{
				if (radio.available())
				{
					radio.read(&messageR, sizeof(messageR));
					Serial.println(" ");
					Serial.print(messageR);
					flag = true;
				}
			}

			Serial.println(" ");
			Serial.print("Message Received from ");
			Serial.print(Range[i]); //1

			/*Serial.println("");
			Serial.print("Message is: ");
			Serial.print(messageR); */
		} 

		Serial.println(" ");
		Serial.println("case 9001 END");
		break;

	case 9002:
		Serial.println(" ");
		Serial.println("case 9002 start");

			flag = false;

			radio.openReadingPipe(0, Station[0]); //0
			radio.startListening();

			Serial.println(" ");
			Serial.print("Receiving from ");
			Serial.print(Station[0]); //0
			Serial.print(" ...");
			
			while (flag == false)
			{
				if (radio.available())
				{
					radio.read(&messageR, sizeof(messageR));
					Serial.println(" ");
					Serial.print(messageR);
					flag = true;
				}
			}

			if (flag == true)
			{
				radio.openWritingPipe(MyAddress);
				radio.stopListening();
				
				Serial.println(" ");
				Serial.print("Sending to ");
				Serial.print(Station[0]); //0
				Serial.print(" ...");
				
				time = millis();
				time = time + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
				while (radio.write(&messageS, sizeof(messageS)) != true)
				{
					if (millis() > time)
					{
						Error(3, Range[0]);
					}
				}

				Serial.println("");
				Serial.print("Message sent to ");
				Serial.print(Station[0]); //0
			}

		Serial.println("");
		Serial.println("case 9002 END");
		break;

	case 9003:
		Serial.println(" ");
		Serial.println("case 9003 start");

		flag = false;

		radio.openReadingPipe(0, Station[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(Range[0]); //0
		Serial.print(" ...");

		while (flag == false)
		{
			if (radio.available())
			{
				radio.read(&messageR, sizeof(messageR));
				Serial.println(" ");
				Serial.print(messageR);
				flag = true;
			}
		}

		if (flag == true)
		{
			radio.openWritingPipe(MyAddress);
			radio.stopListening();

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Range[0]); //0
			Serial.print(" ...");

			time = millis();
			time = time + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(Range[0]); //0
		}

		Serial.println("");
		Serial.println("case 9003 END");
		break;

	case 9004:
		Serial.println(" ");
		Serial.println("case 9004 start");

		flag = false;

		radio.openReadingPipe(0, Station[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(Station[0]); //0
		Serial.print(" ...");

		while (flag == false)
		{
			if (radio.available())
			{
				radio.read(&messageR, sizeof(messageR));
				Serial.println(" ");
				Serial.print(messageR);
				flag = true;
			}
		}

		if (flag == true)
		{
			radio.openWritingPipe(MyAddress);
			radio.stopListening();

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Station[0]); //0
			Serial.print(" ...");

			time = millis();
			time = time + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(Station[0]); //0
		}

		Serial.println("");
		Serial.println("case 9004 END");
		break;

	case 9005:
		Serial.println(" ");
		Serial.println("case 9005 start");

		flag = false;

		radio.openReadingPipe(0, Station[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(Station[0]); //0
		Serial.print(" ...");

		while (flag == false)
		{
			if (radio.available())
			{
				radio.read(&messageR, sizeof(messageR));
				Serial.println(" ");
				Serial.print(messageR);
				flag = true;
			}
		}

		if (flag == true)
		{
			radio.openWritingPipe(MyAddress);
			radio.stopListening();

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Station[0]); //0
			Serial.print(" ...");

			time = millis();
			time = time + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(Station[0]); //0
		}

		Serial.println("");
		Serial.println("case 9005 END");
		break;

	case 9006:
		Serial.println(" ");
		Serial.println("case 9006 start");

		flag = false;

		radio.openReadingPipe(0, Station[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(Station[0]); //0
		Serial.print(" ...");

		while (flag == false)
		{
			if (radio.available())
			{
				radio.read(&messageR, sizeof(messageR));
				Serial.println(" ");
				Serial.print(messageR);
				flag = true;
			}
		}

		if (flag == true)
		{
			radio.openWritingPipe(MyAddress);
			radio.stopListening();

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Station[0]); //0
			Serial.print(" ...");

			time = millis();
			time = time + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(Station[0]); //0
		}

		Serial.println("");
		Serial.println("case 9006 END");
		break;

	case 9007:
		Serial.println(" ");
		Serial.println("case 9007 start");

		flag = false;

		radio.openReadingPipe(0, Station[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(Station[0]); //0
		Serial.print(" ...");

		while (flag == false)
		{
			if (radio.available())
			{
				radio.read(&messageR, sizeof(messageR));
				Serial.println(" ");
				Serial.print(messageR);
				flag = true;
			}
		}

		if (flag == true)
		{
			radio.openWritingPipe(MyAddress);
			radio.stopListening();

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Station[0]); //0
			Serial.print(" ...");

			time = millis();
			time = time + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(Station[0]); //0
		}

		Serial.println("");
		Serial.println("case 9007 END");
		break;

	//Need to add case 9008
	}
}


int DigitCheck(int X)
{
	int Digits = 0;

	Digits = X < 10 ? 1 :
		X < 100 ? 2 :
		X < 1000 ? 3 :
		X < 10000 ? 4 :
		X < 100000 ? 5 : -10;

	if (Digits < 0)
	{
		Error(4);
	}

	else
	{
		return(Digits);
	}
}


void M_Convert(int Num)
{
	
	String Holder = "";
	int Digits = 0;

	//Make sure the incoming number isnt above the max value the String can handle
	//Until I find out why, the max is 29999
	if (Num > 29999)
	{
		Error(4);
	}

	Digits = DigitCheck(Num);

	//Serial.println("DIGITS");
	//Serial.println(Digits); //Debug Only

	Holder = Num;

	for (int i = 0; i != Digits; i++)
	{
		S_Motion[i] = Holder[i];
	}

	//S_Motion[0] = Holder[0];
	//S_Motion[1] = Holder[1];
	//S_Motion[2] = Holder[2];
	////S_Motion[3] = Holder[3];
	////S_Motion[4] = Holder[4];

	//Serial.println("^^^^^^^^");
	//Serial.println(S_Motion); //Debug Only

}


void T_Convert(int Num)
{
	String Holder = "  ";
	int Digits = 0;

	//Make sure the incoming number isnt above the max value the String can handle
	//Until I find out why, the max is 29999
	if (Num > 29999)
	{
		Error(4);
	}

	Digits = DigitCheck(Num);

	Holder = Num;

	for (int i = 0; i != Digits; i++)
	{
		S_Temperature[i] = Holder[i];
	}

	//S_Temperature[0] = Holder[0];
	//S_Temperature[1] = Holder[1];
	//S_Temperature[2] = Holder[2];
	//S_Temperature[3] = Holder[3];
	//S_Temperature[4] = Holder[4];

	//Serial.println(S_Temperature); //Debug Only
}


void Temperature(int Num, int FROMaddress)
{
	switch (FROMaddress)
	{
		case 9001:
			temperature_01 = Num;

		case 9002:
			temperature_02 = Num;

		case 9003:
			temperature_03 = Num;

		case 9004:
			temperature_04 = Num;

		case 9005:
			temperature_05 = Num;

		case 9006:
			temperature_06 = Num;

		case 9007:
			temperature_07 = Num;
	}
}


void Motion(int Num, int FROMaddress)
{
	switch (FROMaddress)
	{
	case 9001:
		MotionCount_01 = Num;

	case 9002:
		MotionCount_02 = Num;

	case 9003:
		MotionCount_03 = Num;

	case 9004:
		MotionCount_04 = Num;

	case 9005:
		MotionCount_05 = Num;

	case 9006:
		MotionCount_06 = Num;

	case 9007:
		MotionCount_07 = Num;
	}
}


void CommandCheck(int command)
{
	//Blank for now

	Serial.println(" ");  //temp.
	Serial.print("Doing Command");  //temp.
}


void DataReceive()
{
	int TOaddress = 0;
	int FROMaddress = 0;
	int AmountofData = 0;
	int message_int = 0; //Setup variable that will hold the incoming integer message data
	char message_str; //Setup variable that will hold the incoming string message data

	radio.read(&message_int, sizeof(message_int)); //Get who this is addressed to

	TOaddress = message_int;

	Serial.print("TOaddress: ");
	Serial.println(message_int);

	//Will only read the remaing message if its addressed to itsself
	if (TOaddress == MyAddress)
	{

		////////////////Get sender's address////////////////////////////	
		while (radio.available() != true) {} //Wait until a signal is available

		radio.read(&message_int, sizeof(message_int));//Get sender's address

		FROMaddress = message_int;

		Serial.print("FROMaddress: ");
		Serial.println(message_int);
		///////////////////////////////////////////////////////////////


		/////////////////Get AmountofData////////////////////////////	
		while (radio.available() != true) {} //Wait until a signal is available

		radio.read(&message_int, sizeof(message_int));//Get the number of how many messages will be sent

		AmountofData = message_int;

		Serial.print("AmountofData: ");
		Serial.println(message_int);
		///////////////////////////////////////////////////////////////


		/////////////// RECEIVE THE DATA ////////////////////////////
		for (int i = 0; i != AmountofData; i++)
		{
			while (radio.available() != true) {} //Wait until a signal is available

			radio.read(&message_str, sizeof(message_str));//Get what type of message will be sent (Motion, Temperature or Random)

			Serial.print("TYPE: ");
			Serial.println(message_str);

			if (message_str == 'M' || message_str == ' M' || message_str == 'M ' || message_str == ' M ')
			{
				while (radio.available() != true) {} //Wait until a signal is available
				radio.read(&message_int, sizeof(message_int));//Get the the motion data

				Motion(message_int, FROMaddress);
			}

			else if (message_str == 'T' || message_str == ' T' || message_str == 'T ' || message_str == ' T ')
			{
				while (radio.available() != true) {} //Wait until a signal is available
				radio.read(&message_int, sizeof(message_int));//Get the temperature data

				Temperature(message_int, FROMaddress);
			}

			else if (message_str == 'R' || message_str == ' R' || message_str == 'R ' || message_str == ' R ')
			{
				while (radio.available() != true) {} //Wait until a signal is available
				radio.read(&message_int, sizeof(message_int));//Get command

				CommandCheck(message_int);

			}

			else
			{
				Error(5, FROMaddress);
			}
		}

		Serial.println(" ");  //Debug Only
		Serial.print("END");  //Debug Only

	}
}