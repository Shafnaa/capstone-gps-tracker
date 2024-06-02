#include "TinyGPSPlus.h"
#include "WiFi.h"
#include "HTTPClient.h"

// The TinyGPSPlus object

TinyGPSPlus gps;

const char *ssid = "OPPO A92";
const char *password = "okayyyyy";

String serverName = "https://ycppywtjigaamvdfjfge.supabase.co/rest/v1/tracks";
const char *apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InljcHB5d3RqaWdhYW12ZGZqZmdlIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MTYzMDMxOTEsImV4cCI6MjAzMTg3OTE5MX0.T-WZ2m4MEMV_7aHcv0R5Md9Iixj1gQU-CmSPRaYrKuA";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 15000;

void setup()
{
  Serial.begin(115200);

  Serial2.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 15 seconds (timerDelay variable), it will take 15 seconds before publishing the first reading.");

  delay(15000);
}

void loop()
{
  // Send an HTTP POST request every 15 seconds
  if ((millis() - lastTime) > timerDelay)
  {
    while (Serial2.available() > 0)
      if (gps.encode(Serial2.read()))
        // Check WiFi connection status
        if (WiFi.status() == WL_CONNECTED)
        {
          postData();
        }
        else
        {
          Serial.println("WiFi Disconnected");
        }
    lastTime = millis();
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));

    while (true)
      ;
  }
}

void postData()
{
  Serial.print(F("Location:"));

  if (gps.location.isValid())
  {
    HTTPClient http;

    String serverPath = serverName;

    double lon = gps.location.lng();
    double lat = gps.location.lat();

    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());

    // If you need Node-RED/server authentication, insert user and password below
    http.setAuthorization("Bearer", apiKey);
    http.addHeader("apiKey", apiKey);

    // Send HTTP GET request
    http.addHeader("Content-Type", "application/json");

    String httpRequestData = "{\"lon\":" + String(lon, 6U) + ",\"lat\":" + String(lat, 6U) + "}";

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);

      Serial.println(httpRequestData);

      Serial.print("Lat: ");
      Serial.print(lat, 6);
      Serial.print("Lon: ");
      Serial.print(lon, 6);
      Serial.println();
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else
  {
    Serial.println(F("INVALID"));
  }
}
