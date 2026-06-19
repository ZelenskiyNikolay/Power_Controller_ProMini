#pragma once
#include <Arduino.h>
#include "I2CSlave.h"

struct Note
{
    uint16_t frequency; // Частота в Гц (0 — пауза)
    uint16_t duration;  // Длительность звука в мс
    uint16_t delayNext; // Пауза ПОСЛЕ этой ноты до следующей в мс
};
class Sound
{
public:
    enum SoundEffect
    {
        FX_NONE,
        FX_WELCOME,
        FX_SHUTDOWN,
        FX_CHARGING, // Звук подключения питания
        FX_NO_CHARGING,
        FX_FULL_CHARGING,
        FX_MENU_CLICK,      // Короткий клик меню
        FX_MENU_CLICK_Q,    // Тихий клик
        FX_ERROR,           // Ошибка / Низкий заряд
        FX_R2D2             // Твой старый добрый рандом
    };

private:
    int pin;
    long timer;
    int longSound;
    uint8_t _volume = 10;

    // Переменные для проигрывания готовых массивов (плейлистов)
    SoundEffect currentEffect;
    const Note *currentPlaylist;
    int playlistSize;
    int currentNoteIndex;

    long timer_i2c;

public:
    Sound() {}
    void begin(int piezoPin);

    bool IsBusy(){return (currentEffect != FX_NONE);}
    void Update(unsigned long dt);
    void RtDt(int Long);
    void playEffect(SoundEffect fx);
    void SoundStop();
};