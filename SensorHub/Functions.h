//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//	The nRF24L01 and DHT libries used in this code are coppyrighted by their respective owners	//
//					and are issued under the GNU General Public License version 2.				//
//																							    //
//**********************************************************************************************//


//SetReadPipes
void SetReadRange()
{

	if (MyAddress == STATION[0])
	{
		Range[0] = MyAddress + 1;
		Range[1] = MyAddress + 2;
		Range[2] = MyAddress + 3;
		Range[3] = MyAddress + 4;
		Range[4] = MyAddress + 5;
		//Range[5] = MyAddress + 6;
	}

	else if (MyAddress == STATION[1])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress + 1;
		Range[2] = MyAddress + 2;
		Range[3] = MyAddress + 3;
		Range[4] = MyAddress + 4;
		//Range[5] = MyAddress + 5;
	}

	else if (MyAddress == STATION[2])
	{
		Range[0] = MyAddress - 2;
		Range[1] = MyAddress - 1;
		Range[2] = MyAddress + 1;
		Range[3] = MyAddress + 2;
		Range[4] = MyAddress + 3;
		//Range[5] = MyAddress + 4;
	}

	else if (MyAddress == STATION[3])
	{
		Range[0] = MyAddress - 3;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 1;
		Range[3] = MyAddress + 1;
		Range[4] = MyAddress + 2;
		//Range[5] = MyAddress + 3;
	}

	else if (MyAddress == STATION[4])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress + 1;
		Range[4] = MyAddress + 2;
		//Range[5] = MyAddress + 3;
	}

	else if (MyAddress == STATION[5])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress - 4;
		Range[4] = MyAddress + 1;
		//Range[5] = MyAddress + 2;
	}

	else if (MyAddress == STATION[6])
	{
		Range[0] = MyAddress - 1;
		Range[1] = MyAddress - 2;
		Range[2] = MyAddress - 3;
		Range[3] = MyAddress - 4;
		Range[4] = MyAddress - 5;
		//Range[5] = MyAddress + 1;
	}

	else if (MyAddress == STATION[7])
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

	bool A = digitalRead(SWITCH_IN_1);
	bool B = digitalRead(SWITCH_IN_2);
	bool C = digitalRead(SWITCH_IN_3);

	if (A == false && B == false && C == false)
	{
		MyAddress = STATION[0]; //9001

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == true && B == false && C == false)
	{
		MyAddress = STATION[1]; //9002

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == false && B == true && C == false)
	{
		MyAddress = STATION[2]; //9003

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == true && B == true && C == false)
	{
		MyAddress = STATION[3]; //9004

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == false && B == false && C == true)
	{
		MyAddress = STATION[4]; //9005

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == true && B == false && C == true)
	{
		MyAddress = STATION[5]; //9006

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == false && B == true && C == true)
	{
		MyAddress = STATION[6]; //9007

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}

	else if (A == true && B == true && C == true)
	{
		MyAddress = STATION[7]; //9008

		//Set time before each delay
		CycleTime = (SEND_TIME + (BASE_CYCLE_TIME * (MyAddress - 9000)));
	}
}


void Error(int errorCode, int Address = 0) //Function that will take an error number and display the proper error code and LED light
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
		Serial.print(Address);
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
		Serial.print(Address);
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


/*void SendText(char message[] = {0}, int address = 0)
{
	radio.stopListening();
	radio.openWritingPipe(MyAddress);
	

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


void SendData(int ToAddress = 0, int Motion = 0, int Temperature = 0, String Command = "nul", int TypeOfData = 0)
{
	radio.stopListening();
	radio.openWritingPipe(MyAddress);

	digitalWrite(8, HIGH);

	//int data[3] = {}; //Have to create 4 arrays otherwise it will cause a weird bug...

	//int nul = 999;
	//int Number = 0;

	Serial.println("  ");
	Serial.println("/////////////");
	Serial.println(ToAddress);
	Serial.println(TypeOfData);
	//Serial.println(Type[0]);
	//Serial.println(data[0]);


	///////////Send the address it wants to send to/////
	unsigned long PastTime = millis();
	PastTime = PastTime + 5000;

	while (radio.write(&ToAddress, sizeof(ToAddress)) != true)
	{
		if (millis() > PastTime)
		{
			Error(3, ToAddress);
		}
	}
	delay(1000);

	///////////Send my address ////////////////////////////
	PastTime = millis();
	PastTime = PastTime + 5000;

	while (radio.write(&MyAddress, sizeof(MyAddress)) != true)
	{
		if (millis() > PastTime)
		{
			Error(3, ToAddress);
		}
	}
	delay(1000);

	///////////Tell the receiver how much data we are sending ////////////////////////////
	PastTime = millis();
	PastTime = PastTime + 5000;

	while (radio.write(&TypeOfData, sizeof(TypeOfData)) != true)
	{
		if (millis() > PastTime)
		{
			Error(3, ToAddress);
		}
	}
	delay(1000);

	///////////////////Send the data////////////////////////////
	PastTime = millis();
	PastTime = PastTime + 5000;

	switch (TypeOfData)
	{
	case 1: //T (Temperature data)
		while (radio.write(&Temperature, sizeof(Temperature)) != true)
		{
			if (millis() > PastTime)
			{
				Error(3, ToAddress);
			}
		}
		delay(1000);

		Serial.println(" ");
		Serial.print("Message(s) sent to ");
		Serial.print(ToAddress);

		break;

	case 2: //M (motion data)
		while (radio.write(&Motion, sizeof(Motion)) != true)
		{
			if (millis() > PastTime)
			{
				Error(3, ToAddress);
			}
		}
		delay(1000);

		Serial.println(" ");
		Serial.print("Message(s) sent to ");
		Serial.print(ToAddress);

		break;

	case 3: //B (motion and temperature data)
		while (radio.write(&Temperature, sizeof(Temperature)) != true)
		{
			if (millis() > PastTime)
			{
				Error(3, ToAddress);
			}
		}
		delay(1000);


		while (radio.write(&Motion, sizeof(Motion)) != true)
		{
			if (millis() > PastTime)
			{
				Error(3, ToAddress);
			}
		}
		delay(1000);

		Serial.println(" ");
		Serial.print(Temperature);
		Serial.print(" & ");
		Serial.print(Motion);
		Serial.print(" sent to ");
		Serial.println(ToAddress);

		break;

	case 4: //C (command data)
		while (radio.write(&Command, sizeof(Command)) != true)
		{
			if (millis() > PastTime)
			{
				Error(3, ToAddress);
			}
		}
		delay(1000);

		Serial.println(" ");
		Serial.print("Message(s) sent to ");
		Serial.print(ToAddress);

		break;
	}

	digitalWrite(8, LOW);

}


void FirstPing()
{
	const String MESSAGE_TO_SEND[] = { "ping" };
	String ReceivedMessage = "";
	int PastTime = 0;
	bool MessageSent = false;

	switch (MyAddress)
	{
	case 9001:
		Serial.println(" ");
		Serial.println("case 9001 start");
		for (int i = 0; i != 2; i++)//'i'= how many devices it will ping. // needs to be set one of the following values / '0' ==> 0 devices / '1' ==> 1 device / '2' ==> 2 devices / and so on... Default # is 6
		{

			PastTime = 0;

			MessageSent = false;

			radio.stopListening();
			radio.openWritingPipe(MyAddress);
			

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Range[i]); //1
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 4000;
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
				{
					//Error(3, Range[i]);
				}
			}

			Serial.println(" ");
			Serial.print("Message sent to ");
			Serial.print(Range[i]); //1 



			radio.openReadingPipe(0, STATION[1]); //Start listening for station2
			radio.startListening();

			Serial.println(" ");
			Serial.print("Receiving from ");
			Serial.print(Range[i]); //1
			Serial.print(" ...");

			while (MessageSent == false)
			{
				if (radio.available())
				{
					radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
					Serial.println(" ");
					Serial.print(ReceivedMessage);
					MessageSent = true;
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

		MessageSent = false;

		radio.openReadingPipe(0, STATION[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(STATION[0]); //0
		Serial.print(" ...");

		while (MessageSent == false)
		{
			if (radio.available())
			{
				radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
				Serial.println(" ");
				Serial.print(ReceivedMessage);
				MessageSent = true;
			}
		}

		if (MessageSent == true)
		{
			radio.stopListening();
			radio.openWritingPipe(MyAddress);

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(STATION[0]); //0
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 30000; //All the non-master hubs get a longer error PastTime to help make sure the master hub gets the responce 
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(STATION[0]); //0
		}

		Serial.println("");
		Serial.println("case 9002 END");
		break;

	case 9003:
		Serial.println(" ");
		Serial.println("case 9003 start");

		MessageSent = false;

		radio.openReadingPipe(0, STATION[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(Range[0]); //0
		Serial.print(" ...");

		while (MessageSent == false)
		{
			if (radio.available())
			{
				radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
				Serial.println(" ");
				Serial.print(ReceivedMessage);
				MessageSent = true;
			}
		}

		if (MessageSent == true)
		{
			radio.stopListening();
			radio.openWritingPipe(MyAddress);

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Range[0]); //0
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 30000; //All the non-master hubs get a longer error PastTime to help make sure the master hub gets the responce 
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
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

		MessageSent = false;

		radio.openReadingPipe(0, STATION[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(STATION[0]); //0
		Serial.print(" ...");

		while (MessageSent == false)
		{
			if (radio.available())
			{
				radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
				Serial.println(" ");
				Serial.print(ReceivedMessage);
				MessageSent = true;
			}
		}

		if (MessageSent == true)
		{
			radio.stopListening();
			radio.openWritingPipe(MyAddress);

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(STATION[0]); //0
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 30000; //All the non-master hubs get a longer error PastTime to help make sure the master hub gets the responce 
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(STATION[0]); //0
		}

		Serial.println("");
		Serial.println("case 9004 END");
		break;

	case 9005:
		Serial.println(" ");
		Serial.println("case 9005 start");

		MessageSent = false;

		radio.openReadingPipe(0, STATION[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(STATION[0]); //0
		Serial.print(" ...");

		while (MessageSent == false)
		{
			if (radio.available())
			{
				radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
				Serial.println(" ");
				Serial.print(ReceivedMessage);
				MessageSent = true;
			}
		}

		if (MessageSent == true)
		{
			radio.stopListening();
			radio.openWritingPipe(MyAddress);

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(STATION[0]); //0
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 30000; //All the non-master hubs get a longer error PastTime to help make sure the master hub gets the responce 
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(STATION[0]); //0
		}

		Serial.println("");
		Serial.println("case 9005 END");
		break;

	case 9006:
		Serial.println(" ");
		Serial.println("case 9006 start");

		MessageSent = false;

		radio.openReadingPipe(0, STATION[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(STATION[0]); //0
		Serial.print(" ...");

		while (MessageSent == false)
		{
			if (radio.available())
			{
				radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
				Serial.println(" ");
				Serial.print(ReceivedMessage);
				MessageSent = true;
			}
		}

		if (MessageSent == true)
		{
			radio.stopListening();
			radio.openWritingPipe(MyAddress);

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(STATION[0]); //0
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 30000; //All the non-master hubs get a longer error PastTime to help make sure the master hub gets the responce 
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(STATION[0]); //0
		}

		Serial.println("");
		Serial.println("case 9006 END");
		break;

	case 9007:
		Serial.println(" ");
		Serial.println("case 9007 start");

		MessageSent = false;

		radio.openReadingPipe(0, STATION[0]); //0
		radio.startListening();

		Serial.println(" ");
		Serial.print("Receiving from ");
		Serial.print(STATION[0]); //0
		Serial.print(" ...");

		while (MessageSent == false)
		{
			if (radio.available())
			{
				radio.read(&ReceivedMessage, sizeof(ReceivedMessage));
				Serial.println(" ");
				Serial.print(ReceivedMessage);
				MessageSent = true;
			}
		}

		if (MessageSent == true)
		{
			radio.stopListening();
			radio.openWritingPipe(MyAddress);

			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(STATION[0]); //0
			Serial.print(" ...");

			PastTime = millis();
			PastTime = PastTime + 30000; //All the non-master hubs get a longer error time to help make sure the master hub gets the responce 
			while (radio.write(&MESSAGE_TO_SEND, sizeof(MESSAGE_TO_SEND)) != true)
			{
				if (millis() > PastTime)
				{
					Error(3, Range[0]);
				}
			}

			Serial.println("");
			Serial.print("Message sent to ");
			Serial.print(STATION[0]); //0
		}

		Serial.println("");
		Serial.println("case 9007 END");
		break;

		//Need to add case 9008
	}
}


/*int DigitCheck(int X)
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
}*/


/*void M_Convert(int Num)
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

}*/


/*void T_Convert(int Num)
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
}*/


void Temperature(int Num = 0, int FROMaddress = 0)
{
	switch (FROMaddress)
	{
	case 9001:
		Hub01Temperature = Num;

	case 9002:
		Hub02Temperature = Num;

	case 9003:
		Hub03Temperature = Num;

	case 9004:
		Hub04Temperature = Num;

	case 9005:
		Hub05Temperature = Num;

	case 9006:
		Hub06Temperature = Num;

	case 9007:
		Hub07Temperature = Num;
	}
}


void Motion(int Num = 0, int FROMaddress = 0)
{
	switch (FROMaddress)
	{
	case 9001:
		Hub01Motion = Num;

	case 9002:
		Hub02Motion = Num;

	case 9003:
		Hub03Motion = Num;

	case 9004:
		Hub04Motion = Num;

	case 9005:
		Hub05Motion = Num;

	case 9006:
		Hub06Motion = Num;

	case 9007:
		Hub07Motion = Num;
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
	int ToAddress = 0;
	int FromAddress = 0;
	int TypeOfData = 0;
	int IntegerMessage = 0; //Setup variable that will hold the incoming integer message data
	//char message_str; //Setup variable that will hold the incoming string message data

	radio.read(&IntegerMessage, sizeof(IntegerMessage)); //Get who this is addressed to

	ToAddress = IntegerMessage;

	//Serial.print("TOaddress: ");
	//Serial.println(message_int);

	//Will only read the remaing message if its addressed to itsself
	if (ToAddress == MyAddress)
	{

		////////////////Get sender's address////////////////////////////	
		while (radio.available() != true) {} //Wait until a signal is available

		IntegerMessage = 0;

		radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get sender's address

		FromAddress = IntegerMessage;

		//Serial.print("FROMaddress: ");
		//Serial.println(message_int);
		///////////////////////////////////////////////////////////////


		/////////////////Get TypeofData////////////////////////////	
		while (radio.available() != true) {} //Wait until a signal is available

		IntegerMessage = 0;
		radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get the number of how many messages will be sent

		TypeOfData = IntegerMessage;

		//Serial.print("AmountofData: ");
		//Serial.println(message_str);
		///////////////////////////////////////////////////////////////


		/////////////// RECEIVE THE DATA ////////////////////////////

			//while (radio.available() != true) {} //Wait until a signal is available

		//	radio.read(&message_str, sizeof(message_str));//Get what type of message will be sent (Motion, Temperature or Random)

			//Serial.print("TYPE: ");
			//Serial.println(message_str);

		if (TypeOfData == 2) // M
		{
			while (radio.available() != true) {} //Wait until a signal is available
			IntegerMessage = 0;
			radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get the the motion data

			Motion(IntegerMessage, FromAddress);
		}

		else if (TypeOfData == 1) // T
		{
			while (radio.available() != true) {} //Wait until a signal is available
			IntegerMessage = 0;
			radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get the temperature data

			Temperature(IntegerMessage, FromAddress);
		}

		else if (TypeOfData == 3) // B 
		{
			while (radio.available() != true) {} //Wait until a signal is available
			IntegerMessage = 0;
			radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get the temperature data

			Temperature(IntegerMessage, FromAddress);

			while (radio.available() != true) {} //Wait until a signal is available
			IntegerMessage = 0;
			radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get the motion data

			Motion(IntegerMessage, FromAddress);

			Serial.println(" ");
			Serial.println("/////////// ");
			Serial.println(IntegerMessage);

		}

		else if (TypeOfData == 4) // C
		{
			while (radio.available() != true) {} //Wait until a signal is available
			IntegerMessage = 0;
			radio.read(&IntegerMessage, sizeof(IntegerMessage));//Get command

			CommandCheck(IntegerMessage);
		}

		else
		{

			Serial.print("TYPE: ");
			Serial.println(TypeOfData);

			Error(5, FromAddress);
		}

		Serial.println(" ");  //Debug Only
		Serial.println("END");  //Debug Only

	}
}