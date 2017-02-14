#pragma once

class Hub
{
public:
	//--------- Constructor ------------//
	Hub();

	//--------- Modifiers -------------//
	
	bool logTemperature(float data);

	bool logMotion(int data);

	//---------- Accessors -----------//
	float getTemerature() const;

	float getMotion() const;

private:
	int temperature;
	int motion;
};