#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include <Crypto.h>
#include <wiringPi.h>
#include <RF24.h>
#include <AES.h>

int main()
{
   AES256 test;
   RF24 radio(9, 10);

   radio.begin();
}
