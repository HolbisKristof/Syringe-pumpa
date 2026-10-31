#include "DisplayManager.h"

DisplayManager::DisplayManager() : u8g2(U8G2_R0, U8X8_PIN_NONE) {}

void DisplayManager::begin() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
}

void DisplayManager::renderSplashAnim() {
  const char* text = "Syringe Pumpa";
  int textLen = strlen(text);
  
  // 1. BEÚSZÓ ANIMÁCIÓ (0.5 sec)
  unsigned long start = millis();
  while (millis() - start < 500) {
    float progress = (float)(millis() - start) / 500.0;
    u8g2.clearBuffer();
    for (int i = 0; i < textLen; i++) {
      int targetX = 10 + (i * 8);
      int startY = -10;
      int targetY = 36;
      int currentY = startY + (targetY - startY) * min(1.0f, progress * (1.0f + i * 0.1f));
      u8g2.drawGlyph(targetX, currentY, text[i]);
    }
    u8g2.sendBuffer();
  }

  // 2. KÖZÉPSŐ MENTÉSI PAUZA (1.0 sec)
  u8g2.clearBuffer();
  u8g2.drawStr(12, 36, text);
  u8g2.sendBuffer();
  delay(1000);

  // 3. KIÚSZÓ ANIMÁCIÓ (0.5 sec)
  start = millis();
  while (millis() - start < 500) {
    float progress = (float)(millis() - start) / 500.0;
    u8g2.clearBuffer();
    for (int i = 0; i < textLen; i++) {
      int targetX = 10 + (i * 8);
      int currentY = 36 + (int)(progress * 40);
      u8g2.drawGlyph(targetX, currentY, text[i]);
    }
    u8g2.sendBuffer();
  }
  
  currentState = STATE_MAIN_MENU;
}

void DisplayManager::updateUI(MotorController& motor, LimitSwitchModule& limit, TempModule& temp, bool wifiActive, String ipStr) {
  u8g2.clearBuffer();

  switch (currentState) {
    case STATE_MAIN_MENU:     drawMainMenu(); break;
    case STATE_SET_SYRINGE:  drawSetSyringeMenu(); break;
    case STATE_SET_SPEED:    drawSetSpeedMenu(); break;
    case STATE_RUNNING:      drawRunningScreen(motor); break;
    case STATE_WIFI_TOGGLE:  drawWifiMenu(wifiActive, ipStr); break;
    case STATE_DEBUG_ROOT:   drawDebugRoot(); break;
    case STATE_DEBUG_TEMP:   drawDebugTemp(temp); break;
    case STATE_DEBUG_LIMITS: drawDebugLimits(limit); break;
    case STATE_DEBUG_MOTOR:  drawDebugMotor(motor); break;
    case STATE_DEBUG_WIFI:   drawDebugWifi(wifiActive, ipStr); break;
    default: break;
  }

  u8g2.sendBuffer();
}

void DisplayManager::handleEncoder(int delta) {
  if (currentState == STATE_MAIN_MENU) {
    menuCursor = constrain(menuCursor + delta, 0, 3);
  } else if (currentState == STATE_SET_SYRINGE) {
    selectedSyringeIdx = constrain(selectedSyringeIdx + delta, 0, SYRINGE_DB_SIZE - 1);
  } else if (currentState == STATE_SET_SPEED) {
    flowVolume = max(0.1f, flowVolume + delta * 0.5f);
  } else if (currentState == STATE_DEBUG_ROOT) {
    menuCursor = constrain(menuCursor + delta, 0, 3);
  }
}

void DisplayManager::handleButtons(bool psh, bool con, bool bak, MotorController& motor) {
  if (psh) {
    if (pshPressStart == 0) pshPressStart = millis();
    if (millis() - pshPressStart >= 3000) pshHeld3Sec = true;
  } else {
    pshPressStart = 0;
  }

  if (pshHeld3Sec && bak) {
    pshHeld3Sec = false;
    currentState = STATE_DEBUG_ROOT;
    menuCursor = 0;
    return;
  }

  if (bak) {
    if (currentState != STATE_MAIN_MENU) currentState = STATE_MAIN_MENU;
    return;
  }

  if (con) {
    if (currentState == STATE_MAIN_MENU) {
      if (menuCursor == 0) currentState = STATE_SET_SYRINGE;
      else if (menuCursor == 1) currentState = STATE_SET_SPEED;
      else if (menuCursor == 2) {
        currentState = STATE_RUNNING;
        runStartTime = millis();
        motor.setRunning(true);
        motor.setEnabled(true);
      }
      else if (menuCursor == 3) currentState = STATE_WIFI_TOGGLE;
    } else if (currentState == STATE_RUNNING) {
      motor.setRunning(false);
      motor.setEnabled(false);
      currentState = STATE_MAIN_MENU;
    } else if (currentState == STATE_DEBUG_ROOT) {
      if (menuCursor == 0) currentState = STATE_DEBUG_TEMP;
      else if (menuCursor == 1) currentState = STATE_DEBUG_LIMITS;
      else if (menuCursor == 2) currentState = STATE_DEBUG_MOTOR;
      else if (menuCursor == 3) currentState = STATE_DEBUG_WIFI;
    }
  }
}

void DisplayManager::drawMainMenu() {
  u8g2.drawStr(0, 10, "--- FO MENU ---");
  u8g2.drawStr(10, 24, menuCursor == 0 ? "> 1. FECSKENDE MERET" : "  1. FECSKENDE MERET");
  u8g2.drawStr(10, 36, menuCursor == 1 ? "> 2. ADAGOLASI SEBESSEG" : "  2. ADAGOLASI SEBESSEG");
  u8g2.drawStr(10, 48, menuCursor == 2 ? "> 3. ADAGOLAS INDITASA" : "  3. ADAGOLAS INDITASA");
  u8g2.drawStr(10, 60, menuCursor == 3 ? "> 4. WIFI BE/KI" : "  4. WIFI BE/KI");
}

void DisplayManager::drawSetSyringeMenu() {
  u8g2.drawStr(0, 10, "--- MERET VALASZTAS ---");
  u8g2.setCursor(0, 32);
  u8g2.print("MERET: ");
  u8g2.print(SYRINGE_DATABASE[selectedSyringeIdx].volumeMl);
  u8g2.print(" ml");
  u8g2.setCursor(0, 48);
  u8g2.print("ALAP: ");
  u8g2.print(SYRINGE_DATABASE[selectedSyringeIdx].areaMm2);
  u8g2.print(" mm2");
}

void DisplayManager::drawSetSpeedMenu() {
  u8g2.drawStr(0, 10, "--- SEBESSEG BEALLITAS ---");
  u8g2.setCursor(0, 32);
  u8g2.print("HOZAM: ");
  u8g2.print(flowVolume, 1);
  u8g2.print(selectedVolUnit == UNIT_ML ? " ml/" : " ul/");
  u8g2.print(selectedTimeUnit == TIME_MIN ? "min" : "sec");
}

void DisplayManager::drawRunningScreen(MotorController& motor) {
  u8g2.drawStr(0, 10, "--- ADAGOLAS FOLYIK ---");
  u8g2.setCursor(0, 24);
  u8g2.print("SEB: "); u8g2.print(flowVolume, 1); u8g2.print(" ml/min");
  u8g2.setCursor(0, 36);
  u8g2.print("MERET: "); u8g2.print(SYRINGE_DATABASE[selectedSyringeIdx].volumeMl); u8g2.print(" ml");
  
  unsigned long elapsedSec = (millis() - runStartTime) / 1000;
  u8g2.setCursor(0, 50);
  u8g2.print("IDO: "); u8g2.print(elapsedSec); u8g2.print(" sec");
}

void DisplayManager::drawWifiMenu(bool active, String ip) {
  u8g2.drawStr(0, 10, "--- WIFI STATUSZ ---");
  u8g2.setCursor(0, 26); u8g2.print("SSID: "); u8g2.print(WIFI_SSID);
  u8g2.setCursor(0, 40); u8g2.print("IP: "); u8g2.print(ip);
  u8g2.setCursor(0, 54); u8g2.print("JELSZO: "); u8g2.print(WIFI_PASS);
}

void DisplayManager::drawDebugRoot() {
  u8g2.drawStr(0, 10, "=== DEBUG MENU ===");
  u8g2.drawStr(10, 24, menuCursor == 0 ? "> HOMERO" : "  HOMERO");
  u8g2.drawStr(10, 36, menuCursor == 1 ? "> LIMIT SWITCHES" : "  LIMIT SWITCHES");
  u8g2.drawStr(10, 48, menuCursor == 2 ? "> MOTOROK" : "  MOTOROK");
  u8g2.drawStr(10, 60, menuCursor == 3 ? "> WIFI INFO" : "  WIFI INFO");
}

void DisplayManager::drawDebugTemp(const TempModule& temp) {
  u8g2.drawStr(0, 10, "[DEBUG: HOMERO]");
  u8g2.setCursor(0, 30); u8g2.print("RAW ADC: "); u8g2.print(temp.getRawADC());
  u8g2.setCursor(0, 46); u8g2.print("VENTILATOR: "); u8g2.print(temp.getFanDutyPercent()); u8g2.print("%");
}

void DisplayManager::drawDebugLimits(const LimitSwitchModule& limit) {
  u8g2.drawStr(0, 10, "[DEBUG: LIMITS]");
  u8g2.setCursor(0, 30); u8g2.print("HOME: "); u8g2.print(limit.isHomePressed() ? "ON" : "OFF");
  u8g2.setCursor(0, 46); u8g2.print("END : "); u8g2.print(limit.isEndPressed() ? "ON" : "OFF");
}

void DisplayManager::drawDebugMotor(const MotorController& motor) {
  u8g2.drawStr(0, 10, "[DEBUG: MOTOR]");
  u8g2.setCursor(0, 26); u8g2.print("FREQ: "); u8g2.print(motor.getStepFrequencyHz()); u8g2.print(" Hz");
  u8g2.setCursor(0, 40); u8g2.print("STEPS/MM: "); u8g2.print(motor.getStepsPerMm());
}

void DisplayManager::drawDebugWifi(bool active, String ip) {
  u8g2.drawStr(0, 10, "[DEBUG: WIFI]");
  u8g2.setCursor(0, 30); u8g2.print("IP: "); u8g2.print(ip);
  u8g2.setCursor(0, 46); u8g2.print("PASS: "); u8g2.print(WIFI_PASS);
}
