#pragma once
#include <Arduino.h>

enum LedName
{
    LED_BOARD = 0,   // Встроенный D13
    LED_DIGITAL_PWR, // Индикатор питания (A2)
    LED_IN_POWER,    // Наличие внешних 5В
    LED_CHARGE,      // Процесс зарядки
    LED_FULL,        // Заряд окончен
    LED_NUM_LEDS     // Всего диодов
};

class LED_Control
{
private:
    enum LedMode
    {
        MODE_OFF,
        MODE_ON,
        MODE_BLINK,
        MODE_ONCE
    };
    struct LedPinState
    {
        uint8_t pin;
        LedMode mode;
        long timer;
        int blinkOnTime;
        int blinkOffTime;
        bool currentLevel;
    };

    LedPinState _leds[LED_NUM_LEDS];

    LED_Control() {};
    LED_Control(const LED_Control &) = delete;
    LED_Control &operator=(const LED_Control &) = delete;

public:
    static LED_Control &getInstance()
    {
        static LED_Control instance;
        return instance;
    }

    void begin()
    {
        uint8_t defaultPins[LED_NUM_LEDS] = {13, 16, 9, 8, 7};
        begin(defaultPins);
    }
    void begin(uint8_t pin[LED_NUM_LEDS]);

    void update(unsigned long dt); // Вызывается в loop, крутит таймеры

    // Твоё АПИ: простое, как автомат Калашникова
    void on(LedName name);
    void off(LedName name);
    void offAll(){for (int i = 0; i < LED_NUM_LEDS; i++) off((LedName)i);}
    void blink(LedName name, int onTimeMs, int offTimeMs);
    void onForTime(LedName name, int timeMs);
    bool isBusy(LedName name) const
    {
        return (_leds[name].mode == MODE_BLINK || _leds[name].mode == MODE_ONCE);
    }
};