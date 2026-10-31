#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// 1. PIN KIOSZTÁSOK (ESP32 WEMOS LOLIN32)
// ==========================================

// OLED Kijelző (I2C)
#define PIN_SDA           26
#define PIN_SCL           22

// Rotary Encoder & Előlapi gombok
#define PIN_ENC_A          4  // CLK
#define PIN_ENC_B         16  // DT (Átkötve GPIO 16-ra)
#define PIN_ENC_SW         2  // PSH gomb
#define PIN_BTN_CON       13  // Confirm / Start-Stop gomb
#define PIN_BTN_BAK       15  // Back gomb

// Végálláskapcsolók
#define PIN_LIMIT_HOME    27  // Kezdőpont (Home)
#define PIN_LIMIT_END     14  // Végpont (End)

// SA-12 Hőmérő & PWM Ventilátor
#define PIN_SA12_AO       34  // ADC1 Analóg bemenet
#define PIN_SA12_DO        5  // Digitális komparátor
#define PIN_FAN_PWM       19  // MOSFET Gate PWM (100Hz)

// Dupla Léptetőmotorok (Közös vezérlés)
#define PIN_MOTOR_STEP    18  // Közös STEP pulse
#define PIN_MOTOR_DIR     23  // Közös DIR irány
#define PIN_MOTOR_ENABLE  17  // Közös ENABLE (Active LOW)

// ==========================================
// 2. HARDVERES ÉS MÉRÉSI KONSTANSOK
// ==========================================

// Wi-Fi Access Point adatok
#define WIFI_SSID         "SyringePump_AP"
#define WIFI_PASS         "12345678"
#define DEBUG_WEB_PASS    "x"

// Hőmérsékleti kalibrációs nyers ADC értékek (SA-12 NTC)
const int ADC_50C = 1500;  // ~50°C -> 20% PWM
const int ADC_65C = 1200;  // ~65°C -> 50% PWM
const int ADC_85C = 800;   // ~85°C -> 100% PWM

// Fecskendő Rekord Struktúra (1ml - 20ml adatbázis)
struct SyringeProfile {
  uint8_t volumeMl;
  float areaMm2; // Belső keresztmetszeti alapterület
};

const SyringeProfile SYRINGE_DATABASE[] = {
  {20, 317.31}, // ~20.1 mm átmérő
  {10, 162.86}, // ~14.4 mm átmérő
  {5,  113.10}, // ~12.0 mm átmérő
  {3,   70.88}, // ~9.5 mm átmérő
  {2,   62.21}, // ~8.9 mm átmérő
  {1,   17.35}  // ~4.7 mm átmérő (Inzulin/Tuberkulin)
};
const uint8_t SYRINGE_DB_SIZE = sizeof(SYRINGE_DATABASE) / sizeof(SYRINGE_DATABASE[0]);

// Mértékegység Típusok
enum VolumeUnit { UNIT_ML, UNIT_UL };
enum TimeUnit   { TIME_SEC, TIME_MIN, TIME_HOUR };

#endif
