#ifndef TEMP_MODULE_H
#define TEMP_MODULE_H

#include "config.h"

class TempModule {
public:
  TempModule();
  void begin();
  void update();
  
  int getRawADC() const { return rawADC; }
  int getFanDutyPercent() const { return fanDutyPercent; }
  bool isDigitalAlarm() const { return digitalAlarm; }

private:
  int rawADC = 0;
  int fanDutyPercent = 0;
  bool digitalAlarm = false;
};

#endif
