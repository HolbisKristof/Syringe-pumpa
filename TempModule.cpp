#include "TempModule.h"

TempModule::TempModule() {}

void TempModule::begin() {
  pinMode(PIN_SA12_AO, INPUT);
  pinMode(PIN_SA12_DO, INPUT);

  // 100 Hz-es PWM beállítása 2-vezetékes ventilátorhoz
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcAttach(PIN_FAN_PWM, 100, 8);
  #else
    ledcSetup(0, 100, 8);
    ledcAttachPin(PIN_FAN_PWM, 0);
  #endif
}

void TempModule::update() {
  rawADC = analogRead(PIN_SA12_AO);
  digitalAlarm = !digitalRead(PIN_SA12_DO);

  // Hőmérséklet-PWM logika
  if (rawADC > ADC_50C) {
    fanDutyPercent = 0; // 50°C alatt OFF
  } else if (rawADC <= ADC_50C && rawADC > ADC_65C) {
    fanDutyPercent = map(rawADC, ADC_50C, ADC_65C, 20, 50);
  } else if (rawADC <= ADC_65C && rawADC > ADC_85C) {
    fanDutyPercent = map(rawADC, ADC_65C, ADC_85C, 50, 100);
  } else {
    fanDutyPercent = 100; // 85°C felett MAX
  }

  int pwmVal = 0;
  if (fanDutyPercent > 0) {
    pwmVal = map(fanDutyPercent, 1, 100, 80, 255);
  }

  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcWrite(PIN_FAN_PWM, pwmVal);
  #else
    ledcWrite(0, pwmVal);
  #endif
}
