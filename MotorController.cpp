#include "MotorController.h"

MotorController::MotorController() {}

void MotorController::begin() {
  pinMode(PIN_MOTOR_STEP, OUTPUT);
  pinMode(PIN_MOTOR_DIR, OUTPUT);
  pinMode(PIN_MOTOR_ENABLE, OUTPUT);
  
  digitalWrite(PIN_MOTOR_STEP, LOW);
  digitalWrite(PIN_MOTOR_DIR, HIGH);
  setEnabled(false); // Alaphelyzetben letiltva
  recalculateParams();
}

void MotorController::updateHardwareParams(float leadMm, float ratio, float angle, uint16_t ustep) {
  leadPitchMm = leadMm;
  gearRatio = ratio;
  stepAngle = angle;
  microstep = ustep;
  recalculateParams();
}

void MotorController::setSyringeArea(float areaMm2) {
  syringeAreaMm2 = areaMm2;
  recalculateParams();
}

void MotorController::setEnabled(bool enable) {
  digitalWrite(PIN_MOTOR_ENABLE, enable ? LOW : HIGH); // Active LOW
}

void MotorController::setDirection(bool forward) {
  currentDir = forward;
  digitalWrite(PIN_MOTOR_DIR, forward ? HIGH : LOW);
}

void MotorController::recalculateParams() {
  float fullStepsPerRev = 360.0 / stepAngle;
  stepsPerMm = (fullStepsPerRev * microstep * gearRatio) / leadPitchMm;
}

void MotorController::setTargetFlow(float volumeVal, VolumeUnit vUnit, TimeUnit tUnit) {
  // Átszámítás mm3/sec-re
  float mm3Val = (vUnit == UNIT_ML) ? (volumeVal * 1000.0) : volumeVal; // 1 ml = 1000 mm3
  float timeSec = 1.0;
  if (tUnit == TIME_MIN) timeSec = 60.0;
  if (tUnit == TIME_HOUR) timeSec = 3600.0;
  
  float flowMm3PerSec = mm3Val / timeSec;
  
  // Löket sebesség (mm/sec) = Áramlás (mm3/s) / Fecskendő alapterület (mm2)
  float linearSpeedMmPerSec = flowMm3PerSec / syringeAreaMm2;
  
  // Lépésfrekvencia (Hz) = mm/sec * steps/mm
  stepFrequencyHz = linearSpeedMmPerSec * stepsPerMm;
  
  if (stepFrequencyHz > 0) {
    stepIntervalMicros = (unsigned long)(1000000.0 / stepFrequencyHz);
  } else {
    stepIntervalMicros = 0;
  }
}

void MotorController::runStep() {
  if (!motorActive || stepIntervalMicros == 0) return;

  unsigned long currentMicros = micros();
  if (currentMicros - lastStepMicros >= stepIntervalMicros) {
    lastStepMicros = currentMicros;
    digitalWrite(PIN_MOTOR_STEP, HIGH);
    delayMicroseconds(2);
    digitalWrite(PIN_MOTOR_STEP, LOW);
  }
}
