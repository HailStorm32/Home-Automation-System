#pragma once
#include <ctime>
#include <string>
#include <iostream>

using namespace std;

class SysTime
{
public:
	SysTime();
	
	~SysTime();

	/*
	Description:
		Will return time in 12hr format
	Return:
		string -- 12hr time (12:32AM/PM)
	*/
	string getTime();

	/*
	Input:
		return24hr -- true returns hr in 24hr format, false returns in 12hr
	Return:
		int -- 12hr or 24hr format hour
	*/
	int getHour(bool return24hr = true);

	/*
	Return:
		int -- minute
	*/
	int getMin();

	/*
	Return:
		int -- second
	*/
	int getSec();

	/*
	Description:
		Start a timer
	*/
	void startTimer(int seconds);

	/*
	Returns:
		True -- timer has "gone off"
		False -- timer has NOT "gone off"
	*/
	bool timerPassed();

	/*
	Description:
		Clear all timer variables and clear the timer flag
	*/
	void stopTimer();
	
private:
	time_t rawTime;
	clock_t ticker;
	struct tm * timeInfo;

	long timeStarted;
	long timeToStop;
	bool timerStarted;
};