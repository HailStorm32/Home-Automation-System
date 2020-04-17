#include "queue.h"

Queue::Queue()
{
    for(uint8_t indx = 0; indx < arraySize; indx++)
    {
        queueArray[indx] = 0;
    }

    insertIndx = 0;
    firstOutIndx = 0;
}


void Queue::pop()
{
    queueArray[firstOutIndx] = 0;
    
    if((firstOutIndx+1) == arraySize)
    {
        firstOutIndx = 0;
    }
    else
    {
        firstOutIndx++;
    }
}


void Queue::push(uint8_t data)
{
    queueArray[insertIndx] = data;

    if((insertIndx+1) == arraySize)
    {
        insertIndx = 0;
    }
    else
    {
        insertIndx++;
    }
}


uint8_t Queue::front()
{
    return queueArray[firstOutIndx];
}


bool Queue::isEmpty()
{
    for(uint8_t indx = 0; indx < arraySize; indx++)
    {
        if(queueArray[indx] != 0)
        {
            return false;
        }
    }
    return true;
}
