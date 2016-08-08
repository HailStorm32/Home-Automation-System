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


void SendNum(int AmountofData, int data1, int data2, int address)
{
	radio.openWritingPipe(MyAddress);
	radio.stopListening();	
	
	int data[2] = {}; //Have to create 3 arrays otherwise it will cause a weird bug...
	data[0] = data1;
	data[1] = data2;
	data[2] = 0;


	/*Serial.println("");
	Serial.println(AmountofData);
	Serial.println("");
	Serial.println(data1);     //Debug Only
	Serial.println("");
	Serial.println(data2);
	Serial.println("");
	Serial.println(address); */

	
	int nul = 999;

	///////////Send the address it wants to send to/////
	unsigned long time = millis();
	time = time + 5000;

	while (radio.write(&address, sizeof(address)) != true)
	{
		if (millis() > time)
		{
			Error(3, address);
		}
	}

	/////////Start sending the data////////////
	for(int i = 0; i != 2; i++)
	{
		time = millis();
		time = time + 5000;

		//Serial.println("fsfsdfsdfsdffsdfdsf");  //Debug Only
		//Serial.println(data[i]);  //Debug Only

		while (radio.write(&data[i], sizeof(data[i])) != true)
		{
			if (millis() > time)
			{
				Error(3, address);
			}
		}

	}

	//////Send a "nul" to tell other side we are done sending//////////
	time = millis();
	time = time + 5000;

	while (radio.write(&nul, sizeof(nul)) != true)
	{
		if (millis() > time)
		{
			Error(3, address);
		}
	}
	
	Serial.println(" ");
	Serial.print("Message(s) sent to ");
	Serial.print(address);

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
