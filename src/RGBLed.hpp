#ifndef RGBLED_H
#define RGBLED_H

#include <mbed.h>

class RGBLed
{
public:
    RGBLed(PinName rPin, PinName gPin, PinName bPin);

    bool getRed();
    bool getGreen();
    bool getBlue();

    RGBLed& setRed(bool value);
    RGBLed& setGreen(bool value);
    RGBLed& setBlue(bool value);
    RGBLed& set(bool rValue, bool gValue, bool bValue);

private:
    DigitalOut r, g, b;
};

#endif // RGBLED_H
