#include <ArduinoJson.h>
#include "TinyGPSPlus.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "HardwareSerial.h"

// The TinyGPSPlus object
TinyGPSPlus gps;

// Updating location concurrently
TaskHandle_t gpsTask;

HardwareSerial SerialPort(1);

struct location
{
  double lat;
  double lon;
};

const char *ssid = "kamar tengah";
const char *password = "kamar123";

String serverName = "http://192.168.1.48:8080";
String path = "/api.php";
// const char *apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InljcHB5d3RqaWdhYW12ZGZqZmdlIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MTYzMDMxOTEsImV4cCI6MjAzMTg3OTE5MX0.T-WZ2m4MEMV_7aHcv0R5Md9Iixj1gQU-CmSPRaYrKuA";
const char *apiKey = "kepston";

location currentLocation;

JsonDocument request, response;

void setup()
{
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 4, 2);

  WiFi.begin(ssid, password);

  delay(3000);

  xTaskCreatePinnedToCore(
      updateLocation, /* Task function. */
      "GPS Task",     /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &gpsTask,       /* Task handle to keep track of created task */
      0);

  Serial.println("Connecting to WiFi and GPS Signal...");
  while (WiFi.status() != WL_CONNECTED || currentLocation.lat == NULL || currentLocation.lon == NULL)
  {
    delay(500);
    if (!(WiFi.status() != WL_CONNECTED))
      Serial.print("o");
    else if (!(currentLocation.lat == NULL || currentLocation.lon == NULL))
      Serial.print("x");
    else
      Serial.print(".");
  }

  Serial.println("");

  delay(2000);

  Serial.println("Starting...");
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    postData();

    delay(15000);
  }
  else
  {
    Serial.println("WiFi Disconnected");

    Serial.println("Reconnecting...");

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
  }
}

void updateLocation(void *pvParameters)
{
  for (;;)
  {
    while (SerialPort.available())
    {
      if (gps.encode(SerialPort.read()))
      {
        if (gps.location.isValid())
        {
          currentLocation.lat = gps.location.lat();
          currentLocation.lon = gps.location.lng();
        }
        else
          continue;
      }
    };

    delay(500);
  }
}

void postData()
{
  HTTPClient http;

  String serverPath = serverName + path;

  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());

  // If you need Authentication insert your auth credentials here
  // http.setAuthorization("Bearer", apiKey);
  // http.addHeader("apiKey", apiKey);

  // Send HTTP GET request
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + (String)apiKey);

  request["latitude"] = String(currentLocation.lat, 6U);
  request["longitude"] = String(currentLocation.lon, 6U);

  String httpRequestData;

  // Serial.println(http.hasHeader("Authorization"));

  serializeJsonPretty(request, httpRequestData);

  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println(http.getString());
    Serial.println(httpRequestData);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    Serial.println(httpRequestData);
  }

  // Free resources
  http.end();
}