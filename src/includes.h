#pragma once
#include <Arduino.h>
#include <avr/sleep.h>
#include "FpsDT.h"
#include "Voltmeter.h"
#include "I2CSlave.h"
#include "LED_Control.h"
#include "Sound.h"


#define BLINK_ON 1000
#define BLINK_OFF 500

#define BUTTON_PIN 2
#define BUTTON2_PIN 17
#define IN_POWER_PIN 10
#define IN_CHARGE_PIN 11
#define IN_FULL_PIN 12
#define DC_PICO_EN_PIN 6
#define DC_SENSOR_EN_PIN 5
#define DC_SOUND_EN_PIN 4
#define DC_MOVE_EN_PIN 3