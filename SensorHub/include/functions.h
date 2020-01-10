String packMessage(String command, byte targetAddr, byte senderAddr,
        float tempData, int motionData);
void placementMode(byte *addresses, RF24 *radioP);



//=======================================================================
//Description: Begins placement mode. Will continuously ping server and 
//  and report via LED whether or not its within range of server either
//  directly or through nearby hubs.
//
//Arguments:
//  (OUT) addresses -- byte array of 32 address
//  (IN)  radioP -- pointer to radio instance
//
//Return:
//  void
//=======================================================================
void placementMode(byte *addresses, RF24 *radioP)
{
    byte myAddress = addresses[0];
    byte serverAddr = addresses[31];
    
    radioP->stopListening();
    radioP->openWritingPipe(serverAddr);
    radioP->openReadingPipe(1, myAddress);

}




//=========================================================================
//Description: Packs all the data needed to be sent into a 32 char array
//
//Arguments:
//  (IN) targetAddr -- single byte address of where this packet is going
//  (IN) senderAddr -- single byte address of sender address
//  (IN) tempData -- data from the temperature sensor
//  (IN) motionData -- data from the motion sensor
//  (IN) command -- 1-3 char command
//
//Return:
//  Void
//==========================================================================
String packMessage(String command, byte targetAddr, byte senderAddr,
        float tempData = 0, int motionData = 0)
{
    /*
	Follows the following format:
	
    targetAddr -> spacer -> command -> tempData -> motionData ->
        senderAddr -> end char
	
    Array index#-------> 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    Array index slot -->[2,5,5,-,S,-,7,0,.,3,- , 1, 2, -, 2, 4, 5, &]
	
    255-S-70.3-12-245&
    */

    char packedMsg [32] = {};
    String numberHolder;
    String packedStr;
    byte indxCounter = 0;
    byte indxStart = 0;

    for(byte indx = 0; indx < 32; indx++)
    {
        packedMsg[indx] = '0';
    }

    //Add targetAddr to message array
    numberHolder = static_cast<String>(targetAddr);     
    for(byte indx = 0; indx < numberHolder.length(); indx++)
    {
        packedMsg[indx] = numberHolder[indx];
        indxCounter++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;

    //Add command to message array
    indxStart = indxCounter; 
    for(byte indx = 0; indx < command.length(); indx++)
    {
        packedMsg[indx + indxStart] = command[indx];
        indxCounter++;
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;

    //Add tempData to message array
    numberHolder = static_cast<String>(tempData);
   
    indxStart = indxCounter; 
    //Limit the length of the number to 7 chars (includes decimal)
    if(numberHolder.length() >= 7)
    {
        for(byte indx = 0; indx < 7; indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }
    else
    {
        for(byte indx = 0; indx < numberHolder.length(); indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }

    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    
    //Add motion to message array
    numberHolder = static_cast<String>(motionData);
    
    indxStart = indxCounter; 
    //Limit the length of the number to 7 chars
    if(numberHolder.length() >= 7)
    {
        for(byte indx = 0; indx < 7; indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }
    else
    {
        for(byte indx = 0; indx < numberHolder.length(); indx++)
        {
            packedMsg[indx + indxStart] = numberHolder[indx];
            indxCounter++;
        }
    }
     
    //Add spacer and inciment our postion in the array
    packedMsg[indxCounter] = '-';
    indxCounter++;
    
    indxStart = indxCounter;
    //Add senderAddr to message array
    numberHolder = static_cast<String>(senderAddr);     
    for(byte indx = 0; indx < numberHolder.length(); indx++)
    {
        packedMsg[indx + indxStart] = numberHolder[indx];
        indxCounter++;
    }

    //Add end char
    packedMsg[indxCounter] = '&';

    packedStr = packedMsg;

    return packedStr;
}
