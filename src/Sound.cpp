#include "Sound.h"
#include <avr/pgmspace.h>

// 1. Мелодия iPhone Charging (два быстрых приятных тона вверх)
const Note fxWelcome[] PROGMEM = {
    {262, 80, 100},
    {330, 80, 100},
    {392, 80, 120},
    {523, 120, 140},
    {392, 100, 120},
    {523, 250, 250}};
const Note fxShutdown[] PROGMEM = {
    {523, 100, 120},
    {392, 100, 120},
    {330, 120, 140},
    {262, 150, 180},
    {196, 250, 300}};
const Note fxCharge[] PROGMEM = {
    {370, 150, 50},
    {554, 150, 0}};

const Note fxNoCharge[] PROGMEM = {
    {1200, 90, 100},
    {1600, 150, 100}};

const Note fxFullCharge[] PROGMEM = {
    {262, 80, 100},
    {330, 80, 100},
    {392, 80, 200},
    {262, 80, 100},
    {330, 80, 100},
    {392, 80, 200},
    {262, 80, 100},
    {330, 80, 100},
    {392, 80, 200},
    {523, 120, 140},
    {392, 100, 120},
    {523, 250, 250}};
// 2. Клик меню (очень короткий высокий пик)
const Note fxClick[] PROGMEM = {
    {2100, 150, 170}};
const Note fxClickQuiet[] PROGMEM = {
    {262, 80, 100}};
// 3. Звук ошибки (три унылых тона вниз)
const Note fxError[] PROGMEM = {
    {600, 150, 180},
    {500, 150, 180},
    {400, 300, 0}};

void Sound::begin(int piezoPin)
{
    pin = piezoPin;
    pinMode(pin, OUTPUT);
}

//Обработка звука
void Sound::Update(unsigned long dt)
{
    // timer_i2c -= dt;
    // if (timer_i2c < 0)
    // {
        I2CSlave *i2c = &I2CSlave::getInstance();
        if ((bool)i2c->readRegister(I2CSlave::REG_I2C_SOUND))
        {
            byte r2 = i2c->readRegister(I2CSlave::REG_SOUND_R2D2);
            byte fx = i2c->readRegister(I2CSlave::REG_SOUND_FX);
            if (r2 != 0)
            {
                RtDt(r2);
                i2c->writeRegister(I2CSlave::REG_SOUND_R2D2, 0);
                i2c->writeRegister(I2CSlave::REG_I2C_SOUND, false);
            }
            if (fx != 0)
            {
                playEffect((SoundEffect)fx);
                i2c->writeRegister(I2CSlave::REG_SOUND_FX, 0);
                i2c->writeRegister(I2CSlave::REG_I2C_SOUND, false);
            }
        }
    //     timer_i2c = 1000;
    // }
    if (currentEffect == FX_NONE)
        return;

    // Логика для старого доброго R2D2
    if (currentEffect == FX_R2D2)
    {
        if (longSound > 0)
        {
            timer -= dt;
            if (timer < 0)
            {
                tone(pin, random(600, 1600), random(20, 80));
                timer = random(10, 80);
                longSound--;
            }
        }
        else
        {
            SoundStop();
        }
        return;
    }

    // Логика для упорядоченных плейлистов (iPhone, Клик, Эрор)
    if (currentPlaylist != nullptr)
    {
        timer -= dt;
        if (timer <= 0)
        {
            currentNoteIndex++; // Переходим к следующей ноте

            if (currentNoteIndex < playlistSize)
            {
                Note currentNote;
                // Читаем следующую ноту из памяти PROGMEM
                memcpy_P(&currentNote, &currentPlaylist[currentNoteIndex], sizeof(Note));

                if (currentNote.frequency > 0)
                {
                    tone(pin, currentNote.frequency, currentNote.duration);
                }
                timer = currentNote.delayNext;
            }
            else
            {
                // Плейлист закончился — тушим всё
                SoundStop();
            }
        }
    }
}

void Sound::RtDt(int Long)
{
    SoundStop();
    currentEffect = FX_R2D2;
    longSound = Long;
    timer = random(10, 80);
    tone(pin, random(600, 1600), random(20, 80));
}

void Sound::playEffect(SoundEffect fx)
{
    SoundStop(); // Гасим старый звук, если он играл
    currentEffect = fx;
    currentNoteIndex = 0;

    switch (fx)
    {
    case FX_WELCOME:
        currentPlaylist = fxWelcome;
        playlistSize = sizeof(fxWelcome) / sizeof(Note);
        break;
    case FX_SHUTDOWN:
        currentPlaylist = fxShutdown;
        playlistSize = sizeof(fxShutdown) / sizeof(Note);
        break;
    case FX_CHARGING:
        currentPlaylist = fxCharge;
        playlistSize = sizeof(fxCharge) / sizeof(Note);
        break;
    case FX_NO_CHARGING:
        currentPlaylist = fxNoCharge;
        playlistSize = sizeof(fxNoCharge) / sizeof(Note);
        break;
    case FX_FULL_CHARGING:
        currentPlaylist = fxFullCharge;
        playlistSize = sizeof(fxFullCharge) / sizeof(Note);
        break;
    case FX_MENU_CLICK:
        currentPlaylist = fxClick;
        playlistSize = sizeof(fxClick) / sizeof(Note);
        break;
    case FX_MENU_CLICK_Q:
        currentPlaylist = fxClickQuiet;
        playlistSize = sizeof(fxClickQuiet) / sizeof(Note);
        break;
    case FX_ERROR:
        currentPlaylist = fxError;
        playlistSize = sizeof(fxError) / sizeof(Note);
        break;
    case FX_R2D2:
        RtDt(15); // Запускаем R2D2 на 15 булек
        return;
    default:
        return;
    }

    // Заряжаем первую ноту из PROGMEM
    if (playlistSize > 0)
    {
        Note currentNote;
        // Магия чтения из флеш-памяти:
        memcpy_P(&currentNote, &currentPlaylist[currentNoteIndex], sizeof(Note));

        if (currentNote.frequency > 0)
        {
            tone(pin, currentNote.frequency, currentNote.duration);
        }
        timer = currentNote.delayNext;
    }
}

void Sound::SoundStop()
{
    currentEffect = FX_NONE;
    currentPlaylist = nullptr;
    playlistSize = 0;
    currentNoteIndex = 0;
    longSound = 0;
    timer = 0;
    noTone(pin);
    digitalWrite(pin, LOW);
}
