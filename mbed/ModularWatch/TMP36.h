#include "mbed.h"

//Setup a new class for TMP36 sensor
class TMP36
{
public:
    TMP36(PinName pin);
    TMP36();
    operator float ();
    float read();
private:
//class sets up the AnalogIn pin
    AnalogIn _pin;
};