#include "ArduinoJson.h"

JsonDocument doc;

void setup()
{
  Serial.begin(115200);

  doc["lat"] = -7.427079;
  doc["lon"] = 109.339334;
}

void loop()
{
  serializeJsonPretty(doc, Serial);

  delay(10000);
}