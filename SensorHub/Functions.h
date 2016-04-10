



void SetReadRange()
{

	if (MyAddress == Station1)
	{
		Listen[0] = MyAddress + 1;
		Listen[1] = MyAddress + 2;
		Listen[2] = MyAddress + 3;
		Listen[3] = MyAddress + 4;
		Listen[4] = MyAddress + 5;
		Listen[5] = MyAddress + 6;
	}

	else if (MyAddress == Station2)
	{
		Listen[0] = MyAddress - 1;
		Listen[1] = MyAddress + 1;
		Listen[2] = MyAddress + 2;
		Listen[3] = MyAddress + 3;
		Listen[4] = MyAddress + 4;
		Listen[5] = MyAddress + 5;
	}

	else if (MyAddress == Station3)
	{
		Listen[0] = MyAddress - 2;
		Listen[1] = MyAddress - 1;
		Listen[2] = MyAddress + 1;
		Listen[3] = MyAddress + 2;
		Listen[4] = MyAddress + 3;
		Listen[5] = MyAddress + 4;
	}

	else if (MyAddress == Station4)
	{
		Listen[0] = MyAddress - 3;
		Listen[1] = MyAddress - 2;
		Listen[2] = MyAddress - 1;
		Listen[3] = MyAddress + 1;
		Listen[4] = MyAddress + 2;
		Listen[5] = MyAddress + 3;
	}

	else if (MyAddress == Station5)
	{
		Listen[0] = MyAddress - 1;
		Listen[1] = MyAddress - 2;
		Listen[2] = MyAddress - 3;
		Listen[3] = MyAddress + 1;
		Listen[4] = MyAddress + 2;
		Listen[5] = MyAddress + 3;
	}

	else if (MyAddress == Station6)
	{
		Listen[0] = MyAddress - 1;
		Listen[1] = MyAddress - 2;
		Listen[2] = MyAddress - 3;
		Listen[3] = MyAddress - 4;
		Listen[4] = MyAddress + 1;
		Listen[5] = MyAddress + 2;
	}

	else if (MyAddress == Station7)
	{
		Listen[0] = MyAddress - 1;
		Listen[1] = MyAddress - 2;
		Listen[2] = MyAddress - 3;
		Listen[3] = MyAddress - 4;
		Listen[4] = MyAddress - 5;
		Listen[5] = MyAddress + 1;
	}

	else if (MyAddress == Station8)
	{
		Listen[0] = MyAddress - 1;
		Listen[1] = MyAddress - 2;
		Listen[2] = MyAddress - 3;
		Listen[3] = MyAddress - 4;
		Listen[4] = MyAddress - 5;
		Listen[5] = MyAddress - 6;
	}

	//Set the reading pipes
	radio.openReadingPipe(0, Listen[0]);
	radio.openReadingPipe(1, Listen[1]);
	radio.openReadingPipe(2, Listen[2]);
	radio.openReadingPipe(3, Listen[3]);
	radio.openReadingPipe(4, Listen[4]);
	radio.openReadingPipe(5, Listen[5]);

}


void SetAddress()
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


void Error(int errorCode) //Function that will take an error number and display the proper error code and LED light
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
}

