#include "LimitSwitchModule.h"

LimitSwitchModule::LimitSwitchModule() {}

void LimitSwitchModule::begin() {
  pinMode(PIN_LIMIT_HOME, INPUT_PULLUP);
  pinMode(PIN_LIMIT_END, INPUT_PULLUP);
}

void LimitSwitchModule::update(MotorController& motor) {
  bool currentHome = (digitalRead(PIN_LIMIT_HOME) == LOW);
  bool currentEnd  = (digitalRead(PIN_LIMIT_END) == LOW);

  unsigned long now = millis();

  // Automatikus visszatolatás feldolgozása hiba esetén
  if (autoReverseMode) {
    if (now - autoReverseStart >= 2000) { // 2 másodperces visszahúzás lejárta
      motor.setRunning(false);
      motor.setEnabled(false);
      autoReverseMode = false;
      faultActive = true; // Hibajelzés beállítása
    }
    return;
  }

  // --- HOME LIMIT SWITCH LOGIKA ---
  if (currentHome) {
    if (!homePressed) { // Most nyomták meg először
      homePressed = true;
      homePressStart = now;
      motor.setDirection(!motor.getDirection()); // Irányváltás
    } else { // Folyamatosan nyomva van
      if (now - homePressStart >= 2000) { // 2 mp-nél hosszabb beragadás
        autoReverseMode = true;
        autoReverseStart = now;
        motor.setDirection(true); // Előre fordítás 2 másodpercre
      }
    }
  } else {
    homePressed = false;
  }

  // --- END LIMIT SWITCH LOGIKA ---
  if (currentEnd) {
    if (!endPressed) {
      endPressed = true;
      endPressStart = now;
      motor.setDirection(!motor.getDirection());
    } else {
      if (now - endPressStart >= 2000) {
        autoReverseMode = true;
        autoReverseStart = now;
        motor.setDirection(false); // Hátra fordítás 2 másodpercre
      }
    }
  } else {
    endPressed = false;
  }
}
