#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "config.h"

class MotorController {
public:
  MotorController();
  void begin();
  void updateHardwareParams(float leadMm, float gearRatio, float stepAngle, uint16_t microstep);
  void setSyringeArea(float areaMm2);
  void setTargetFlow(float volumeVal, VolumeUnit vUnit, TimeUnit tUnit);
  
  void setEnabled(bool enable);
  void setDirection(bool forward);
  bool getDirection() const { return currentDir; }
  
  void runStep(); // Hívandó a különálló Core 1 motor task-ban
  
  // Számított adatok elérése
  float getStepsPerMm() const { return stepsPerMm; }
  float getStepFrequencyHz() const { return stepFrequencyHz; }
  bool isRunning() const { return motorActive; }
  void setRunning(bool run) { motorActive = run; }

private:
  float leadPitchMm = 8.0;      // T8 menetes orsó (8mm/fordulat)
  float gearRatio = 1.0;        // Áttétel (1:1)
  float stepAngle = 1.8;        // 200 lépés/fordulat
  uint16_t microstep = 16;      // 1/16 microstepping
  
  float syringeAreaMm2 = 317.31; // Alapértelmezett: 20ml
  float stepsPerMm = 400.0;
  float stepFrequencyHz = 0.0;
  
  volatile bool currentDir = true; // true = Kinyomás, false = Visszahúzás
  volatile bool motorActive = false;
  unsigned long lastStepMicros = 0;
  unsigned long stepIntervalMicros = 0;

  void recalculateParams();
};

#endif
