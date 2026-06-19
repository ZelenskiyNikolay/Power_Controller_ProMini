#include "I2CSlave.h"

void I2CSlave::begin(uint8_t address)
{
    Wire.begin(address);
    Wire.onReceive(onReceiveWrapper);
    Wire.onRequest(onRequestWrapper);
}

void I2CSlave::setBit(uint8_t reg, uint8_t bitNumber, bool value)
{
    if (reg >= REG_NUM_REGS || bitNumber > 7)
        return;

    uint8_t currentVal = _registers[reg];
    if (value)
    {
        currentVal |= (1 << bitNumber); // Взводим бит в 1
    }
    else
    {
        currentVal &= ~(1 << bitNumber); // Сбрасываем бит в 0
    }
    _registers[reg] = currentVal;
}

// Прочитать конкретный бит из конкретного регистра
bool I2CSlave::getBit(uint8_t reg, uint8_t bitNumber) const
{
    if (reg >= REG_NUM_REGS || bitNumber > 7)
        return false;

    return (_registers[reg] & (1 << bitNumber)) != 0;
}

uint8_t I2CSlave::readRegister(uint8_t reg) const
{
    if (reg < REG_NUM_REGS)
        return _registers[reg];
    return 0;
}

void I2CSlave::writeRegister(uint8_t reg, uint8_t value)
{
    if (reg < REG_NUM_REGS)
        _registers[reg] = value;
}

// Врапперы теперь берут единственный экземпляр через getInstance()
void I2CSlave::onReceiveWrapper(int howMany)
{
    I2CSlave::getInstance().handleReceive(howMany);
}

void I2CSlave::onRequestWrapper()
{
    I2CSlave::getInstance().handleRequest();
}

void I2CSlave::handleReceive(int howMany)
{
    LED_Control::getInstance().onForTime(LED_BOARD,50);
    if (howMany < 1)
        return;
    _currentReg = Wire.read();
    while (Wire.available())
    {
        uint8_t data = Wire.read();
        if (_currentReg < REG_NUM_REGS)
        {
            _registers[_currentReg] = data;
        }
    }
}

void I2CSlave::handleRequest()
{
    LED_Control::getInstance().onForTime(LED_BOARD,50);
    if (_currentReg < REG_NUM_REGS)
    {
        Wire.write((uint8_t)_registers[_currentReg]);
    }
    else
    {
        Wire.write(0x00);
    }
}