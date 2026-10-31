#ifndef WEB_MODULE_H
#define WEB_MODULE_H

#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "MotorController.h"
#include "TempModule.h"
#include "LimitSwitchModule.h"

class WebModule {
public:
  WebModule();
  void begin();
  void update();
  String getIPString() const { return WiFi.softAPIP().toString(); }

private:
  WebServer server;
  bool isDebugUnlocked = false;

  void handleRoot();
};

#endif
