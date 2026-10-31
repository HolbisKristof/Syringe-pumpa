#include <Arduino.h>
#include "config.h"
#include "MotorController.h"
#include "LimitSwitchModule.h"
#include "TempModule.h"
#include "DisplayManager.h"
#include "WebModule.h"

// Modulok példányosítása
MotorController motorMod;
LimitSwitchModule limitMod;
TempModule tempMod;
DisplayManager displayMod;
WebModule webMod;

// Encoder változók
volatile int encoderDelta = 0;
volatile byte aFlag = 0, bFlag = 0;

void IRAM_ATTR readEncoderISR() {
  byte readingA = digitalRead(PIN_ENC_A);
  byte readingB = digitalRead(PIN_ENC_B);
  if (readingA != aFlag) {
    aFlag = readingA;
    if (aFlag == HIGH && readingB == LOW) encoderDelta++;
  }
  if (readingB != bFlag) {
    bFlag = readingB;
    if (bFlag == HIGH && readingA == LOW) encoderDelta--;
  }
}

// =========================================================================
// CORE 1: DEDIKÁLT MOTOR TASK (A motor folyamatos lépésképzéséhez)
// =========================================================================
void codeForMotorCore(void * pvParameters) {
  for (;;) {
    motorMod.runStep();
    vTaskDelay(1 / portTICK_PERIOD_MS); // Rendszeridő átadása
  }
}

void setup() {
  Serial.begin(115200);

  // Gombok és megszakítások
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
  pinMode(PIN_BTN_CON, INPUT_PULLUP);
  pinMode(PIN_BTN_BAK, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_ENC_A), readEncoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_B), readEncoderISR, CHANGE);

  // Modulok inicializálása
  motorMod.begin();
  limitMod.begin();
  tempMod.begin();
  displayMod.begin();
  webMod.begin();

  // Kijelző beúszó indító animációja
  displayMod.renderSplashAnim();

  // FreeRTOS Motor Task elindítása dedikáltan a CORE 1 magon
  xTaskCreatePinnedToCore(
    codeForMotorCore,  /* Task függvény */
    "MotorCoreTask",   /* Task név */
    4096,              /* Stack méret */
    NULL,              /* Paraméter */
    10,                /* Prioritás (Magas) */
    NULL,              /* Task handle */
    1                  /* MAG: Core 1 */
  );
}

// =========================================================================
// CORE 0: WEBSZERVER, OLED KIJELZŐ, HŐMÉRŐ, LIMIT SWITCHES
// =========================================================================
void loop() {
  // 1. HARDVERES SENSOREK ÉS WEBSZERVER FRISSÍTÉSE
  tempMod.update();
  limitMod.update(motorMod);
  webMod.update();

  // 2. ADAGOLÁSI PARAMÉTEREK ÁTADÁSA
  uint8_t syrIdx = displayMod.getSelectedSyringeIndex();
  motorMod.setSyringeArea(SYRINGE_DATABASE[syrIdx].areaMm2);
  motorMod.setTargetFlow(displayMod.getFlowVolume(), displayMod.getVolUnit(), displayMod.getTimeUnit());

  // 3. ENKÓDER ÉS GOMBOK KEZELÉSE
  if (encoderDelta != 0) {
    displayMod.handleEncoder(encoderDelta);
    encoderDelta = 0;
  }

  bool btnPsh = !digitalRead(PIN_ENC_SW);
  bool btnCon = !digitalRead(PIN_BTN_CON);
  bool btnBak = !digitalRead(PIN_BTN_BAK);
  displayMod.handleButtons(btnPsh, btnCon, btnBak, motorMod);

  // 4. KIJELZŐ FRISSÍTÉSE
  displayMod.updateUI(motorMod, limitMod, tempMod, true, webMod.getIPString());

  delay(10);
}
