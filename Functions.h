void SetReadPipes(int StationNum)
{



}


void SetAddress()
{
	if (digitalRead(2) == LOW)
	{
		Serial.println("Address #1:");

		MyAddress = Station1;

		Serial.println(MyAddress);
	}

	else if (digitalRead(2) == HIGH)
	{
		Serial.println("Address set to: #2");

		MyAddress = Station2;

		Serial.println(MyAddress);
	}
}

