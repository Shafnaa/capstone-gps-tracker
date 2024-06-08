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

String serverName = "http://sherlockin.000webhostapp.com";
String postPath = "/api.php";
String getPath = "/data.php";
String apiKey = "kepston";

location currentLocation, oldLocation;

JsonDocument request;

int await = 3000, awaitSetup = 3000;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600);
  SerialPort.begin(9600, SERIAL_8N1, 4, 2);

  xTaskCreatePinnedToCore(
      updateLocation, /* Task function. */
      "GPS Task",     /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &gpsTask,       /* Task handle to keep track of created task */
      0);

  // while (!Serial2.available())
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  Serial.println("Configuring GPRS...");
  gsm_send_serial("AT+SAPBR=3,1,Contype,\"GPRS\"", await);
  gsm_send_serial("AT+SAPBR=3,1,APN,\"" + apn + "\"", await);

  delay(15000);
}

void loop()
{
  while (currentLocation.lat == NULL || currentLocation.lon == NULL)
    ;

  postData();
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
          oldLocation.lat = currentLocation.lat;
          oldLocation.lon = currentLocation.lon;

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
  String httpRequestData;

  request["latitude"] = currentLocation.lat;
  request["longitude"] = currentLocation.lon;

  convertFromJson(request, httpRequestData);

  httpPost(httpRequestData);
}

void getState()
{
}

void modemInit()
{
  Serial.println("Configuring GPRS...");
  gsm_send_serial("AT+SAPBR=3,1,Contype,\"GPRS\"", await);
  gsm_send_serial("AT+SAPBR=3,1,APN,\"" + apn + "\"", await);
}

void httpPost(String postData)
{
  Serial.println("Starting post request...");
  gsm_send_serial("AT+SAPBR=1,1", await);
  gsm_send_serial("AT+SAPBR=2,1", await);
  gsm_send_serial("AT+HTTPINIT", await);
  gsm_send_serial("AT+HTTPPARA=\"CID\",1", await);
  gsm_send_serial("AT+HTTPPARA=\"URL\",\"" + serverName + postPath + "\"", await);
  gsm_send_serial("AT+HTTPPARA=\"USERDATA\",\"Authorization: Bearer " + apiKey + "\"", await);
  gsm_send_serial("AT+HTTPPARA=\"CONTENT\",\"application/json\"", await);
  gsm_send_serial("AT+HTTPDATA=" + String(postData.length()) + ",10000", await);
  gsm_send_serial(postData, await);
  gsm_send_serial("AT+HTTPACTION=1", await);
  gsm_send_serial("AT+HTTPREAD", await);
  gsm_send_serial("AT+HTTPTERM", await);
  gsm_send_serial("AT+SAPBR=0,1", await);
}

void httpGet()
{
  Serial.println("Starting get request...");
  gsm_send_serial("AT+SAPBR=1,1", await);
  gsm_send_serial("AT+SAPBR=2,1", await);
  gsm_send_serial("AT+HTTPINIT", await);
  gsm_send_serial("AT+HTTPPARA=\"CID\",1", await);
  gsm_send_serial("AT+HTTPPARA=\"URL\",\"" + serverName + postPath + "\"", await);
  gsm_send_serial("AT+HTTPACTION=0", await);
  gsm_send_serial("AT+HTTPREAD", await);
  gsm_send_serial("AT+HTTPTERM", await);
  gsm_send_serial("AT+SAPBR=0,1", await);
}

void gsm_send_serial(String command, int await)
{
  Serial.println("Send ->: " + command);
  Serial2.println(command);
  long wtimer = millis();
  while (wtimer + await > millis())
  {
    while (Serial2.available())
    {
      Serial.write(Serial2.read());
    }
  }
  Serial.println();
}
