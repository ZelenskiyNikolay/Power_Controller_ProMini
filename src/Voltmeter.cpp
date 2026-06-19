#include "Voltmeter.h"

void Voltmeter::begin(uint8_t pin) {
    this->pin = pin;
}

void Voltmeter::update(unsigned long dt) {
    timer -= dt;
    
    if (timer > 0) return;  
    timer = up_timer;

    int raw = analogRead(pin);
    
    voltage = (raw / 1023.0f) * 1.1f * 4.0f;

    if(_charge)
        voltage +=0.57f;
    // voltage *= 0.95f; //калибровка

    
    Serial.print(getVoltage());//Печать значения в порт
    Serial.println(" V");
}

float Voltmeter::getVoltage() const {
    return voltage;
}