//**********************************************************************************************//
//																								//
//					This code by Demetrius Van Sickle is licensed under a						//
//				Creative Commons Attribution-NonCommercial 4.0 International License.			//
//																								//
//					Credit to Tom Archer & Rick Leinecker for the Serial class					//
//								Source site: https://goo.gl/KftQEJ								//
//																							    //
//**********************************************************************************************//
#include <cstring>
#include "Hub.h"
#include "SysTime.h"

using namespace std;

const unsigned int COM_PORT = 6;

fstream file;
const string COMMANDS_FILE = "Valid_Commands.txt";
const long AUTO_DELAY_TIME = 1800000; //Time between samples when in automode (in milli sec) CURRENT -> 30min

bool isValidCommand(string command);
void runCommand(string command, Hub &masterHub);

void main()
{
	Hub masterHub(COM_PORT);
	string userData;
	bool started = false;

	cout << "----------------Command Center----------------\n\n\n\n";

	do
	{
		cout << "Type 's' to startup:";
		do
		{
			cin >> userData;
		} while (tolower(userData[0]) != 's' && tolower(userData[0]) != 'f');

		cout << "\n\nStarting up..." << endl;

		if (tolower(userData[0]) == 's')
		{
			started = masterHub.begin();
		}
		else
		{
			started = masterHub.begin(false);
		}

	} while (!started);

	userData = "";
	cout << "\n\n\n\n";

	do
	{
		do
		{
			cout << "Please enter a command (type 'help' for help): ";
			cin >> userData;
		} while (!isValidCommand(userData));

		runCommand(userData, masterHub);

	} while (userData != "quit");


}

bool isValidCommand(string givenCommand)
{
	char data[200];
	string command;
	int indx = 0;

	file.open(COMMANDS_FILE);

	while (!file.eof())
	{
		file.getline(data, 200);

		//Get the command from the line text
		while (data[indx] != ' ')
		{
			command += data[indx];
			indx++;
		}
		indx = 0;

		if (command == givenCommand)
		{
			file.close();
			return true;
		}
		command = "";
	}
	file.close();
	cout << "\n '" << givenCommand << "' not a valid command!\n\n" << endl;
	return false;
}

void runCommand(string command, Hub &masterHub)
{
	string codedData;
	char data[200];
	string lineOfText;
	int indx = 0;
	int commandNum = 0;
	SysTime clk;

	if (command == "9001" || command == "9002" || command == "9003")
	{
		codedData = masterHub.requestData(stoi(command));

		masterHub.printData(codedData);
		masterHub.storeData(codedData);
	}
	else if (command == "quit")
	{
		return;
	}
	else if (command == "help")
	{
		file.open(COMMANDS_FILE);

		while (!file.eof())
		{
			file.getline(data, 200);

			while (data[indx] != '<')
			{
				lineOfText += data[indx];
				indx++;
			}

			cout << commandNum + 1 << ") " << lineOfText << endl;

			indx = 0;
			lineOfText = "";
			commandNum++;
		}
		file.close();
		cout << "\n\n\n\n" << endl;
	}
	else if (command == "restart")
	{
		masterHub.restart();
	}
	else if (command == "auto")
	{
		cout << "\n\n\n\nStarting auto sampling. Close program to quit." << endl;
		
		while (true)
		{
			codedData = masterHub.requestData(9001);
			masterHub.storeData(codedData);
			Sleep(1000);
			codedData = masterHub.requestData(9002);
			masterHub.storeData(codedData);
			Sleep(1000);
			codedData = masterHub.requestData(9003);
			masterHub.storeData(codedData);
			Sleep(1000);

			cout << "\n\n\n[" << clk.getTime() << "]Sleeping..." << endl;
			Sleep(AUTO_DELAY_TIME);
		}
	}
}
