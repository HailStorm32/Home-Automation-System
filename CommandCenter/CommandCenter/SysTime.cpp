#include "SysTime.h"

SysTime::SysTime()
{
	timerStarted = false;
	timeStarted = 0;
	timeToStop = 0;
}

SysTime::~SysTime()
{
}

string SysTime::getTime()
{
	string timeToReturn;
	int hour;
	int min;
	bool isPm = false;
	
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	hour = timeInfo->tm_hour;
	min = timeInfo->tm_min;
	
	//Convert to 12hr format
	if (hour > 12)
	{
		hour -= 12;
		isPm = true;
	}

	if (hour <= 0)
	{
		hour = 1;
	}
	
	timeToReturn = to_string(hour);
	timeToReturn += ":";
	timeToReturn += to_string(min);

	//Add the AM or PM
	if (isPm)
	{
		timeToReturn += "PM";
	}
	else
	{
		timeToReturn += "AM";
	}

	return timeToReturn;
}

int SysTime::getHour(bool return24hr)
{
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	
	if (return24hr)
	{
		return timeInfo->tm_hour;
	}
	else
	{
		if (timeInfo->tm_hour > 12)
		{
			return timeInfo->tm_hour - 12;
		}
	}
}

int SysTime::getMin()
{
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	return timeInfo->tm_min;
}

int SysTime::getSec()
{
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	return timeInfo->tm_sec;
}

void SysTime::startTimer(int seconds)
{
	//Can't overwrite a timer that is already running
	if (!timerStarted)
	{
		ticker = clock();

		timeStarted = ticker / CLOCKS_PER_SEC;
		timeToStop = timeStarted + seconds;
		timerStarted = true;
	}
	else
	{
		cout << "\nERROR! Can't start more than one timer!" << endl;
	}
}

bool SysTime::timerPassed()
{
	//Only continue if we have already started the timer
	if (timerStarted)
	{
		ticker = clock();

		if ((ticker / CLOCKS_PER_SEC) >= timeToStop)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		cout << "\nERROR! Start a timer first!" << endl;
		return false;
	}
}

void SysTime::stopTimer()
{
	//Only continue if we have already started the timer
	if (timerStarted)
	{
		timerStarted = false;
		timeStarted = 0;
		timeToStop = 0;
	}
	else
	{
		cout << "\nERROR! Start a timer first!" << endl;
		return;
	}
}
