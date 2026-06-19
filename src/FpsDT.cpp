#include "FpsDT.h"

unsigned long lastTime = 0;
unsigned long currentMillis = 0;
int callsPerSecond = 0;


unsigned long getDeltaTime()
{
  unsigned long now = millis();
  unsigned long dt = now - lastTime;
  lastTime = now;

  return dt;
}

int freeMemory()
{
  char top;
  extern int *__brkval;
  extern int __heap_start;
  return (int)&top - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void FpsCount(unsigned long dt)
{
  callsPerSecond++;
  currentMillis += dt;
  if (currentMillis >= 1000)
  {
    Serial.print("Вызовов в секунду: ");
    Serial.print(callsPerSecond);
    Serial.print(" Память: ");
    Serial.println(freeMemory());
    callsPerSecond = 0;
    currentMillis = 0;
  }
}