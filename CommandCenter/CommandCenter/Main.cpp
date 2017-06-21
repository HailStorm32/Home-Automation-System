//#include "stdafx.h" 
//#include "serial.h"
//#include <iostream>
//#include <string>
#include <cstring>
#include "Hub.h"

using namespace std;

const unsigned int COM_PORT = 4;

void main()
{
	Hub masterHub(COM_PORT);
	
	//masterHub.begin();

	//cout << masterHub.requestData(9002) << endl;
	
	//masterHub.printData(masterHub.requestData(9003));

	masterHub.storeData("9002-74.36-589x");

}
