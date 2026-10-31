#include "WebModule.h"

WebModule::WebModule() : server(80) {}

void WebModule::begin() {
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  
  server.on("/", [this]() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Fecskendos Adagolo</title></head><body>";
    html += "<h1>Syringe Pump Control Interface</h1>";
    html += "<p>Status: ONLINE</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });
  server.begin();
}

void WebModule::update() {
  server.handleClient();
}
