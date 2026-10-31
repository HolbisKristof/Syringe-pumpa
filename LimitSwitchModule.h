#ifndef LIMIT_SWITCH_MODULE_H
#define LIMIT_SWITCH_MODULE_H

#include "config.h"
#include "MotorController.h"

class LimitSwitchModule {
public:
  LimitSwitchModule();
  void begin();
  void update(MotorController& motor);
  
  bool isHomePressed() const { return homePressed; }
  bool isEndPressed() const { return endPressed; }
  bool isFaultState() const { return faultActive; }
  void clearFault() { faultActive = false; }

private:
  bool homePressed = false;
  bool endPressed = false;
  
  unsigned long homePressStart = 0;
  unsigned long endPressStart = 0;
  
  bool faultActive = false;
  bool autoReverseMode = false;
  unsigned long autoReverseStart = 0;
};

#endif
