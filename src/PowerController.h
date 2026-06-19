#pragma once
#include <Arduino.h>
#include "includes.h"

class PowerController
{
public:
    PowerController() : sound(), voltmeter() {}
    void begin();
    void Update(unsigned long dt);

private:
    void SleepMetod();
    void InPower();
    void Charge(unsigned long dt);
    void Full(unsigned long dt);
    void DC_EN();
    void Show_Bt_Level();

    long TIMEOUT = 10000;
    long timer2 = TIMEOUT;
    long timer = 0;
    bool led_state = false;
    bool in_power = false;
    bool charge = false;
    bool full = false;
    bool connectedCharge = false;
    bool compliteCharge = false;

    bool dc1_state = false;
    bool dc2_state = false;
    bool dc3_state = false;
    bool dc4_state = true;

    // переменные антидребезга
    bool bt_debounce = false;
    bool bt2_debounce = false;
    int charge_integrator;
    long charge_timer;
    int full_integrator;
    long full_timer;
    
    Sound sound;
    Voltmeter voltmeter;
};