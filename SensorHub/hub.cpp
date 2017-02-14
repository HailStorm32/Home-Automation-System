#include "hub.h"

Hub::Hub()
{
	temperature = 0;
	motion = 0;
}

bool Hub::logTemperature(float data)
{
	return false;
}

bool Hub::logMotion(int data)
{
	return false;
}

float Hub::getTemerature() const
{
	return 0;
}

float Hub::getMotion() const
{
	return 0;
}
