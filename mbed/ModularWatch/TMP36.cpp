#include "mbed.h"
#include "TMP36.h"
 
TMP36::TMP36(PinName pin) : _pin(pin)
{
// _pin(pin) means pass pin to the AnalogIn constructor
}
 
float TMP36::read()
{
//convert sensor reading to temperature in degrees F
    //return ((_pin.read()*3.3)-0.500)*100.0; C
    return ((_pin.read()*541.667) - 66.833);
    //return _pin.read()*100.0;
}

//overload of float conversion (avoids needing to type .read() in equations)
TMP36::operator float ()
{
//convert sensor reading to temperature in degrees F
    //return ((_pin.read()*3.3)-0.500)*100.0; C
    return ((_pin.read()*541.667) - 66.833);
    //return _pin.read()*100.0;
}