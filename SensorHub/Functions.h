const byte switchIn_1 = 2;
const byte switchIn_2 = 3;
const byte switchIn_3 = 4;



void SetReadPipes(int StationNum)
{



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

