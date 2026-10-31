#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <U8g2lib.h>
#include "config.h"
#include "MotorController.h"
#include "LimitSwitchModule.h"
#include "TempModule.h"

enum MenuState {
  STATE_SPLASH,
  STATE_MAIN_MENU,
  STATE_SET_SYRINGE,
  STATE_SET_SPEED,
  STATE_RUNNING,
  STATE_WIFI_TOGGLE,
  STATE_DEBUG_ROOT,
  STATE_DEBUG_TEMP,
  STATE_DEBUG_LIMITS,
  STATE_DEBUG_MOTOR,
  STATE_DEBUG_WIFI
};

class DisplayManager {
public:
  DisplayManager();
  void begin();
  void renderSplashAnim();
  void updateUI(MotorController& motor, LimitSwitchModule& limit, TempModule& temp, bool wifiActive, String ipStr);
  void handleEncoder(int delta);
  void handleButtons(bool psh, bool con, bool bak, MotorController& motor);

  MenuState getCurrentState() const { return currentState; }
  
  uint8_t getSelectedSyringeIndex() const { return selectedSyringeIdx; }
  float getFlowVolume() const { return flowVolume; }
  VolumeUnit getVolUnit() const { return selectedVolUnit; }
  TimeUnit getTimeUnit() const { return selectedTimeUnit; }

private:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;
  MenuState currentState = STATE_SPLASH;
  
  int menuCursor = 0;
  uint8_t selectedSyringeIdx = 0;
  
  float flowVolume = 10.0;
  VolumeUnit selectedVolUnit = UNIT_ML;
  TimeUnit selectedTimeUnit = TIME_MIN;
  
  unsigned long pshPressStart = 0;
  bool pshHeld3Sec = false;

  unsigned long runStartTime = 0;
  
  void drawMainMenu();
  void drawSetSyringeMenu();
  void drawSetSpeedMenu();
  void drawRunningScreen(MotorController& motor);
  void drawWifiMenu(bool active, String ip);
  void drawDebugRoot();
  void drawDebugTemp(const TempModule& temp);
  void drawDebugLimits(const LimitSwitchModule& limit);
  void drawDebugMotor(const MotorController& motor);
  void drawDebugWifi(bool active, String ip);
};

#endif
