#include "PowerController.h"

void PowerController::begin()
{
    pinMode(IN_POWER_PIN, INPUT);
    pinMode(IN_CHARGE_PIN, INPUT_PULLUP);
    pinMode(IN_FULL_PIN, INPUT_PULLUP);
    pinMode(DC_PICO_EN_PIN, OUTPUT);
    pinMode(DC_SENSOR_EN_PIN, OUTPUT);
    pinMode(DC_SOUND_EN_PIN, OUTPUT);
    pinMode(DC_MOVE_EN_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);

    sound.begin(15);
    sound.playEffect(Sound::FX_WELCOME);

    voltmeter.begin(A0);

    I2CSlave::getInstance().begin();

    I2CSlave::getInstance().writeRegister(I2CSlave::REG_BAT_LEVEL, 100);
    I2CSlave::getInstance().writeRegister(I2CSlave::REG_SYS_CONTROL, 0x02);
    I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_CONTROL, 1, false); // on Pico
    I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_CONTROL, 2, true);
    I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_CONTROL, 3, true);
    I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_CONTROL, 4, true);

    LED_Control::getInstance().begin();
    LED_Control::getInstance().blink(LED_DIGITAL_PWR, 1000, 200);
    LED_Control::getInstance().onForTime(LED_IN_POWER, 2000);
    LED_Control::getInstance().onForTime(LED_CHARGE, 1500);
    LED_Control::getInstance().onForTime(LED_FULL, 1000);

    digitalWrite(DC_PICO_EN_PIN, LOW); // Вкл Пико..  вырубаем все потребители..
    digitalWrite(DC_SENSOR_EN_PIN, HIGH);
    digitalWrite(DC_SOUND_EN_PIN, HIGH);
    digitalWrite(DC_MOVE_EN_PIN, HIGH);
}
void PowerController::InPower()
{
    bool in = digitalRead(IN_POWER_PIN);
    if (in != in_power)
    {
        in_power = in;
        if (in_power)
            LED_Control::getInstance().on(LED_IN_POWER);
        else
            LED_Control::getInstance().off(LED_IN_POWER);

        if (in_power)
        {
            if (!connectedCharge)
            {
                connectedCharge = true;
                sound.playEffect(Sound::FX_CHARGING);
            }
        }
        else
        {
            if (connectedCharge)
                sound.playEffect(Sound::FX_NO_CHARGING);
            connectedCharge = false;
            compliteCharge = false;
        }
        I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_STATUS, 0, in_power);
    }
}
void PowerController::Charge(unsigned long dt)
{
    bool ch = !digitalRead(IN_CHARGE_PIN);

    if (ch)
        charge_integrator++;
    else
        charge_integrator--;

    if (charge_integrator > 150)
        charge_integrator = 150;
    else if (charge_integrator < -150)
        charge_integrator = -150;

    charge_timer -= dt;
    if (charge_timer > 0)
        return;
    charge_timer = 500;

    if (charge_integrator > 0)
        charge = true;
    else
        charge = false;

    if (charge)
        LED_Control::getInstance().on(LED_CHARGE);
    else
        LED_Control::getInstance().off(LED_CHARGE);

    I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_STATUS, 1, charge);
}
void PowerController::Full(unsigned long dt)
{
    bool fl = !digitalRead(IN_FULL_PIN);

    if (fl)
        full_integrator++;
    else
        full_integrator--;

    if (full_integrator > 150)
        full_integrator = 150;
    else if (full_integrator < -150)
        full_integrator = -150;

    full_timer -= dt;
    if (full_timer > 0)
        return;
    full_timer = 500;

    if (full_integrator > 0)
        full = true;
    else
        full = false;

    if (full)
        LED_Control::getInstance().on(LED_FULL);
    else
        LED_Control::getInstance().off(LED_FULL);

    if (full)
        if (!compliteCharge)
        {
            compliteCharge = true;
            sound.playEffect(Sound::FX_FULL_CHARGING);
        }
    voltmeter.ch_update(false);
    I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_STATUS, 2, full);
}

int toPercent(float voltage)
{
    // Адаптируй под свой делитель!
    if (voltage > 4.0f)
        return 100;
    if (voltage < 3.40f)
        return 0;

    float percent = (voltage - 3.40f) / (4.15f - 3.40f) * 100.0f;
    return (int)constrain(percent, 0, 100);
}
void PowerController::Show_Bt_Level()
{
    int v = toPercent(voltmeter.getVoltage());
    Serial.print("АКБ: ");
    Serial.print(voltmeter.getVoltage());
    Serial.print("V  ");
    Serial.print("% АКБ: ");
    Serial.print(v);
    Serial.println("%");

    if (v < 31)
    {
        LED_Control::getInstance().onForTime(LED_IN_POWER, 5000);
        return;
    }
    else if (v < 61)
    {
        LED_Control::getInstance().onForTime(LED_IN_POWER, 5000);
        LED_Control::getInstance().onForTime(LED_CHARGE, 5000);
        return;
    }
    else
    {
        LED_Control::getInstance().onForTime(LED_IN_POWER, 5000);
        LED_Control::getInstance().onForTime(LED_CHARGE, 5000);
        LED_Control::getInstance().onForTime(LED_FULL, 5000);
        return;
    }
}
void PowerController::Update(unsigned long dt)
{
    if (charge)
    {
        if (voltmeter.getVoltage() > 4.3f)
            voltmeter.ch_update(false);
        else
            voltmeter.ch_update(true);
    }
    voltmeter.update(dt);
    LED_Control::getInstance().update(dt);
    sound.Update(dt);

    if (I2CSlave::getInstance().getBit(I2CSlave::REG_SYS_CONTROL, 1))
    {
        sound.playEffect(Sound::FX_SHUTDOWN);
        while (sound.IsBusy())
        {
            float dt1 = getDeltaTime();
            sound.Update(dt1);
        }
        SleepMetod();
    }

    timer -= dt;
    if (!in_power)
    {
        if (!bt_debounce)
            if (!digitalRead(BUTTON_PIN))
            {
                bt_debounce = true;
                sound.playEffect(Sound::FX_MENU_CLICK);
                Show_Bt_Level();
            }
        if (!bt2_debounce)
            if (!digitalRead(BUTTON2_PIN))
            {
                bt2_debounce = true;
                sound.playEffect(Sound::FX_SHUTDOWN);
                while (sound.IsBusy())
                {
                    unsigned long dt1 = getDeltaTime();
                    sound.Update(dt1);
                }
                SleepMetod();
            }
    }
    if (timer > 0)
    {

        if (!LED_Control::getInstance().isBusy(LED_IN_POWER))
        {
            InPower();
            Charge(dt);
            Full(dt);
            DC_EN();
            bt_debounce = false;
        }
    }
    else
    {

        I2CSlave::getInstance().writeRegister(I2CSlave::REG_BAT_LEVEL, (byte)(voltmeter.getVoltage() * 50.0f));

        timer = 1000;
    }
}
void PowerController::DC_EN()
{
    bool e1 = I2CSlave::getInstance().getBit(I2CSlave::REG_SYS_CONTROL, 1);
    if (e1 != dc1_state)
    {
        dc1_state = e1;
        digitalWrite(DC_PICO_EN_PIN, dc1_state);
    }
    bool e2 = I2CSlave::getInstance().getBit(I2CSlave::REG_SYS_CONTROL, 2);
    if (e2 != dc2_state)
    {
        dc2_state = e2;
        digitalWrite(DC_SENSOR_EN_PIN, dc2_state);
    }
    bool e3 = I2CSlave::getInstance().getBit(I2CSlave::REG_SYS_CONTROL, 3);
    if (e3 != dc3_state)
    {
        dc3_state = e3;
        digitalWrite(DC_SOUND_EN_PIN, dc3_state);
    }
    bool e4 = I2CSlave::getInstance().getBit(I2CSlave::REG_SYS_CONTROL, 4);
    if (e4 != dc4_state)
    {
        dc4_state = e4;
        digitalWrite(DC_MOVE_EN_PIN, dc4_state);
    }
}
void wakeUpISR() {}
void PowerController::SleepMetod()
{
    LED_Control::getInstance().offAll();
    digitalWrite(DC_PICO_EN_PIN, HIGH); // вырубаем все потребители..
    digitalWrite(DC_SENSOR_EN_PIN, HIGH);
    digitalWrite(DC_SOUND_EN_PIN, HIGH);
    digitalWrite(DC_MOVE_EN_PIN, HIGH);

    LED_Control::getInstance().on(LED_BOARD);

    Serial.println("Ухожу в глубокий сон...");
    Serial.flush(); // Ждем, пока Serial полностью отправит текст, иначе он зависнет

    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Выбираем самый суровый режим энергосбережения
    sleep_enable();                      // Разрешаем архитектуре сон

    // Привязываем прерывание к пину D2.
    // В режиме PWR_DOWN гарантированно будит только режим LOW (низкий уровень на пине)
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUpISR, LOW);

    // 3. Засыпаем! (Контроллер замирает на этой строчке)
    sleep_cpu();

    // ──── ЗДЕСЬ ПРОЦЕССОР СТОИТ И ЖДЕТ НАЖАТИЯ КНОПКИ ────

    // 4. ПРОСНУЛИСЬ! Как только нажали кнопку, код продолжается отсюда:
    sleep_disable();                                    // Первым делом запрещаем сон
    detachInterrupt(digitalPinToInterrupt(BUTTON_PIN)); // Отключаем прерывание, чтобы не спамило

    LED_Control::getInstance().blink(LED_DIGITAL_PWR, 1000, 200); // Включаем индикатор питания

    LED_Control::getInstance().off(LED_BOARD);

    if (!dc1_state)
        digitalWrite(DC_PICO_EN_PIN, dc1_state); // возвращаем состояние до сна
    else
        I2CSlave::getInstance().setBit(I2CSlave::REG_SYS_CONTROL, 1, false); // on Pico

    Serial.println("Доброе утро! Я проснулся!");

    sound.playEffect(Sound::FX_WELCOME);
    while (sound.IsBusy())
    {
        float dt1 = getDeltaTime();
        sound.Update(dt1);
    }
    bt2_debounce = false;
}
