//**************************************************************************************//
//																						//
//			This work by Demetrius Van Sickle is licensed under a						//
//		Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																					    //
//**************************************************************************************//


//SetReadPipes
/*void SetReadRange()
{

	if (MyAddress == Station1)
	{
		Listen0 = MyAddress + 1;
		Listen1 = MyAddress + 2;
		Listen2 = MyAddress + 3;
		Listen3 = MyAddress + 4;
		Listen4 = MyAddress + 5;
		Listen5 = MyAddress + 6;
	}

	else if (MyAddress == Station2)
	{
		Listen0 = MyAddress - 1;
		Listen1 = MyAddress + 1;
		Listen2 = MyAddress + 2;
		Listen3 = MyAddress + 3;
		Listen4 = MyAddress + 4;
		Listen5 = MyAddress + 5;
	}

	else if (MyAddress == Station3)
	{
		Listen0 = MyAddress - 2;
		Listen1 = MyAddress - 1;
		Listen2 = MyAddress + 1;
		Listen3 = MyAddress + 2;
		Listen4 = MyAddress + 3;
		Listen5 = MyAddress + 4;
	}

	else if (MyAddress == Station4)
	{
		Listen0 = MyAddress - 3;
		Listen1 = MyAddress - 2;
		Listen2 = MyAddress - 1;
		Listen3 = MyAddress + 1;
		Listen4 = MyAddress + 2;
		Listen5 = MyAddress + 3;
	}

	else if (MyAddress == Station5)
	{
		Listen0 = MyAddress - 1;
		Listen1 = MyAddress - 2;
		Listen2 = MyAddress - 3;
		Listen3 = MyAddress + 1;
		Listen4 = MyAddress + 2;
		Listen5 = MyAddress + 3;
	}

	else if (MyAddress == Station6)
	{
		Listen0 = MyAddress - 1;
		Listen1 = MyAddress - 2;
		Listen2 = MyAddress - 3;
		Listen3 = MyAddress - 4;
		Listen4 = MyAddress + 1;
		Listen5 = MyAddress + 2;
	}

	else if (MyAddress == Station7)
	{
		Listen0 = MyAddress - 1;
		Listen1 = MyAddress - 2;
		Listen2 = MyAddress - 3;
		Listen3 = MyAddress - 4;
		Listen4 = MyAddress - 5;
		Listen5 = MyAddress + 1;
	}

	else if (MyAddress == Station8)
	{
		Listen0 = MyAddress - 1;
		Listen1 = MyAddress - 2;
		Listen2 = MyAddress - 3;
		Listen3 = MyAddress - 4;
		Listen4 = MyAddress - 5;
		Listen5 = MyAddress - 6;
	}

}*/


void SetAddress() //Decode signal from OR gate switch and assign address 
{
	
	delay(2000);

	bool A = digitalRead(switchIn_1);
	bool B = digitalRead(switchIn_2);
	bool C = digitalRead(switchIn_3);

	if (A == false && B == false && C == false)
	{
		MyAddress = Station1;
	}

	else if (A == true && B == false && C == false)
	{
		MyAddress = Station2;
	}

	else if (A == false && B == true && C == false)
	{
		MyAddress = Station3;
	}

	else if (A == true && B == true && C == false)
	{
		MyAddress = Station4;
	}

	else if (A == false && B == false && C == true)
	{
		MyAddress = Station5;
	}

	else if (A == true && B == false && C == true)
	{
		MyAddress = Station6;
	}

	else if (A == false && B == true && C == true)
	{
		MyAddress = Station7;
	}

	else if (A == true && B == true && C == true)
	{
		MyAddress = Station8;
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
}


/*void SendText(char message[] = {0}, int address = 0)
{

	int time = millis();
	time + 10000;
	while (radio.write(&message, sizeof(message)) != true)
	{
		if (millis() > time)
		{
			//Error(3, address);
		}
	}
}*/


/*void SendNum(float number, int address = 0)
{
	int time = millis();
	time + 10000;
	while (radio.write(&number, sizeof(number)) != true)
	{
		if (millis() > time)
		{
			Error(3, address);
		}
	}
} */


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
		//for (int i = 0; i != 1; i++)// 'i' needs to be set to 1 more than the max # of values// Default # is 6
		//{
			flag = false;

			radio.openWritingPipe(MyAddress);
			radio.stopListening();
			
			Serial.println(" ");
			Serial.print("Sending to ");
			Serial.print(Station2);
			Serial.print(" ...");
			
			time = millis();
			time = time + 1000;
			while (radio.write(&messageS, sizeof(messageS)) != true)
			{
				if (millis() > time)
				{
					Error(3);
				}
			}

			Serial.println(" ");
			Serial.print("Message sent to ");
			Serial.print(Station2);



			radio.openReadingPipe(0, Station2); //Start listening for station2
			radio.startListening();

			Serial.println(" ");
			Serial.print("Receiving from ");
			Serial.print(Station2);
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
			Serial.print(Station2);

			/*Serial.println("");
			Serial.print("Message is: ");
			Serial.print(messageR); */
		//} 

		Serial.println(" ");
		Serial.println("case 9001 END");
		break;

	case 9002:
		Serial.println(" ");
		Serial.println("case 9002 start");
	//	for (int i = 0; i != 1; i++)// 'i' needs to be set to 1 more than the max # of values// Default # is 6
		//{
			flag = false;

			radio.openReadingPipe(0, Station1);
			radio.startListening();

			Serial.println(" ");
			Serial.print("Receiving from ");
			Serial.print(Station1);
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
				Serial.print(Station1);
				Serial.print(" ...");
				
				time = millis();
				time = time + 1000;
				while (radio.write(&messageS, sizeof(messageS)) != true)
				{
					if (millis() > time)
					{
						Error(3);
					}
				}

				Serial.println("");
				Serial.print("Message sent to ");
				Serial.print(Station1); 
			}

		//}

		Serial.println("");
		Serial.println("case 9002 END");
		break;
	}
}
