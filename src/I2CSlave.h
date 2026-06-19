#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "LED_Control.h"
#include "Sound.h"

class I2CSlave {
public:
    enum {
        REG_BAT_LEVEL   = 0x01,
        REG_SYS_CONTROL = 0x02,
        REG_SYS_STATUS  = 0x03,
        REG_I2C_SOUND = 0x05,
        REG_SOUND_FX = 0x06,
        REG_SOUND_R2D2 = 0x07,
        REG_NUM_REGS
    };

    static I2CSlave& getInstance() {
        static I2CSlave instance; 
        return instance;
    }

    void begin(uint8_t address = 0x25);
    
    uint8_t readRegister(uint8_t reg) const;
    void writeRegister(uint8_t reg, uint8_t value);

    // Логика прерываний
    void handleReceive(int howMany);
    void handleRequest();

    void setBit(uint8_t reg, uint8_t bitNumber, bool value);
    bool getBit(uint8_t reg, uint8_t bitNumber) const ;
private:
    // 1. Закрываем конструкторы, чтобы никто не мог сделать new или скопировать объект
    I2CSlave() : _currentReg(0) { memset((void*)_registers, 0, REG_NUM_REGS); }
    I2CSlave(const I2CSlave&) = delete;
    I2CSlave& operator=(const I2CSlave&) = delete;

    volatile uint8_t _registers[REG_NUM_REGS];
    volatile uint8_t _currentReg;

    // Статические обработчики прерываний
    static void onReceiveWrapper(int howMany);
    static void onRequestWrapper(); 
};