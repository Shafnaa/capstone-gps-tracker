#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// The TinyGPSPlus object
TinyGPSPlus gps;

// Updating location concurrently
TaskHandle_t gpsTask;

// Assigning GPS to serial port 1
HardwareSerial SerialPort(1);

struct location
{
  double lat;
  double lon;
};

String apn = "TSEL-SNS";

String serverName = "https://ycppywtjigaamvdfjfge.supabase.co";
String pathName = "/rest/v1/tracks";
String apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InljcHB5d3RqaWdhYW12ZGZqZmdlIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MTYzMDMxOTEsImV4cCI6MjAzMTg3OTE5MX0.T-WZ2m4MEMV_7aHcv0R5Md9Iixj1gQU-CmSPRaYrKuA";

location currentLocation;

JsonDocument request;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 4, 2);

  delay(3000);

  xTaskCreatePinnedToCore(
      updateLocation, /* Task function. */
      "GPS Task",     /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &gpsTask,       /* Task handle to keep track of created task */
      0);

  modemInit();

  delay(2000);
}

void loop()
{
  while (currentLocation.lat == NULL || currentLocation.lon == NULL)
    ;

  delay(15000);
}

void updateLocation(void *pvParameters)
{
  for (;;)
  {
    while (Serial2.available())
    {
      if (gps.encode(Serial2.read()))
      {
        if (gps.location.isValid())
        {
          currentLocation.lat = gps.location.lat();
          currentLocation.lon = gps.location.lng();

          delay(5000);
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
  String serverPath = serverName + pathName;

  String httpRequestData;

  request["lat"] = currentLocation.lat;
  request["lon"] = currentLocation.lon;

  convertFromJson(request, httpRequestData);

  httpPost(httpRequestData);
}

void modemInit()
{
  Serial.println("Configuring GPRS...");
  gsm_send_serial("AT+SAPBR=3,1,Contype,\"GPRS\"");
  gsm_send_serial("AT+SAPBR=3,1,APN,\"" + apn + "\"");
}

void gsm_send_serial(String command)
{
  Serial.println("Send ->: " + command);
  Serial2.println(command);
  long wtimer = millis();
  while (wtimer + 3000 > millis())
  {
    while (Serial2.available())
    {
      Serial.write(Serial2.read());
    }
  }
  Serial.println();
}

String toJson(location location)
{
  return "{\"lat\":" + String(location.lat, 6U) + ",\"lon\":" + String(location.lon, 6U) + "}";
}

void httpPost(String postData)
{
  Serial.println("Starting post request...");
  gsm_send_serial("AT+SAPBR=1,1");
  gsm_send_serial("AT+SAPBR=2,1");
  gsm_send_serial("AT+HTTPINIT");
  gsm_send_serial("AT+HTTPPARA=\"CID\",1");
  gsm_send_serial("AT+HTTPPARA=\"URL\",\"" + serverName + pathName + "\"");
  gsm_send_serial("AT+HTTPPARA=\"USERDATA\",\"Authorization: Bearer " + apiKey + "\"");
  gsm_send_serial("AT+HTTPPARA=\"USERDATA\",\"apiKey: " + apiKey + "\"");
  gsm_send_serial("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  gsm_send_serial("AT+HTTPDATA=" + String(postData.length()) + ",10000");
  gsm_send_serial(postData);
  gsm_send_serial("AT+HTTPREAD");
  gsm_send_serial("AT+HTTPTERM");
  gsm_send_serial("AT+SAPBR=0,1");
}