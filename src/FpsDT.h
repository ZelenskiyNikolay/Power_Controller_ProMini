#pragma once
#include <Arduino.h>

extern unsigned long lastTime;

extern unsigned long currentMillis;
extern int callsPerSecond;

unsigned long getDeltaTime();
int freeMemory();
void FpsCount(unsigned long dt);

