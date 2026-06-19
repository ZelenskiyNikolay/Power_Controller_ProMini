#include "includes.h"
#include "PowerController.h"

PowerController power;
void setup()
{
  Serial.begin(9600);
  delay(2000);
  Serial.println("Serial started.");
  analogReference(INTERNAL); // Для ATmega328p это включает встроенный ИОН на 1.1В
  randomSeed(analogRead(A7));

  power.begin();
  lastTime = millis();
}
void loop()
{
  unsigned long dt = getDeltaTime();

  power.Update(dt);
  
  FpsCount(dt);
}