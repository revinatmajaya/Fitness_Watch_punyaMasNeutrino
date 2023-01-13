#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define JSON_BUFF_DIMENSION 2500

void initWeather();
String* fetchWeather();
String* parseJson(const char* jsonString);
