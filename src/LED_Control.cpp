#include "LED_Control.h"

void LED_Control::begin(uint8_t pin[LED_NUM_LEDS])
{
    for (int i = 0; i < LED_NUM_LEDS; i++)
    {
        _leds[i].pin = pin[i];
        _leds[i].mode = MODE_OFF;
        _leds[i].timer = 0;
        _leds[i].blinkOnTime = 0;
        _leds[i].blinkOffTime = 0;
        _leds[i].currentLevel = LOW;    
        pinMode(_leds[i].pin, OUTPUT);
        digitalWrite(_leds[i].pin, _leds[i].currentLevel);
    }
}

void LED_Control::on(LedName name) {
    if(_leds[name].mode == MODE_ON) return;
    _leds[name].mode = MODE_ON;
    _leds[name].currentLevel = HIGH;
    digitalWrite(_leds[name].pin, HIGH);
}

void LED_Control::off(LedName name) {
    if(_leds[name].mode == MODE_OFF) return;
    _leds[name].mode = MODE_OFF;
    _leds[name].currentLevel = LOW;
    digitalWrite(_leds[name].pin, LOW);
}
// Настройка мигания (параметры передаем в мс)
void LED_Control::blink(LedName name, int onTimeMs, int offTimeMs) {
    _leds[name].mode = MODE_BLINK;
    _leds[name].blinkOnTime = onTimeMs;
    _leds[name].blinkOffTime = offTimeMs;
    if (_leds[name].timer <= 0) {
        _leds[name].timer = onTimeMs;
        _leds[name].currentLevel = HIGH;
        digitalWrite(_leds[name].pin, HIGH);
    }
}
// Включить на определенное время (в мс)
void LED_Control::onForTime(LedName name, int timeMs) {
    _leds[name].mode = MODE_ONCE;
    _leds[name].timer = timeMs;
    _leds[name].currentLevel = HIGH;
    digitalWrite(_leds[name].pin, HIGH);
}
// Сердце класса — молотит таймеры на основе dt
void LED_Control::update(unsigned long dt) {
    for (int i = 0; i < LED_NUM_LEDS; i++) {
        LedPinState& led = _leds[i];

        if (led.mode == MODE_OFF || led.mode == MODE_ON) continue;

        led.timer -= dt; // Вычитаем прошедшее время (dt у тебя в мс)

        if (led.timer <= 0) {
            if (led.mode == MODE_BLINK) {
                // Меняем состояние инверсией
                led.currentLevel = !led.currentLevel;
                // Заряжаем таймер на нужную фазу
                led.timer = led.currentLevel ? led.blinkOnTime : led.blinkOffTime;
                digitalWrite(led.pin, led.currentLevel);
            } 
            else if (led.mode == MODE_ONCE) {
                // Время одиночной вспышки вышло — тушим
                led.mode = MODE_OFF;
                led.currentLevel = LOW;
                digitalWrite(led.pin, LOW);
            }
        }
    }
}