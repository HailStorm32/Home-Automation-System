#include "Arduino.h"

class Queue
{
    public:
        Queue();

        void pop();

        void push(uint8_t data);

        uint8_t front();

        bool isEmpty();

    private:

        const uint8_t arraySize = 32;
        uint8_t queueArray[32];
        uint8_t firstOutIndx;
        uint8_t insertIndx;
};
