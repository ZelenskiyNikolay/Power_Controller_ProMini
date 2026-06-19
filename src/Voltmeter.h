#pragma once
#include <Arduino.h>

class Voltmeter {
public:
    Voltmeter(){}

    void begin(uint8_t pin);
    void update(unsigned long dt);
    void ch_update(bool charge){_charge = charge;}
    float getVoltage() const;
private:
    uint8_t pin;
    float voltage = 0;
    long up_timer = 3000;
    long timer;
    bool _charge = false;
};
