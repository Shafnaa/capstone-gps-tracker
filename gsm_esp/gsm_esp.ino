String apn = "TSEL-SNS";

String serverName = "http://sherlockin.000webhostapp.com";
String postPath = "/api.php";
String getPath = "/data.php";
String apiKey = "kepston";

struct location
{
  double lat;
  double lon;
};

int await = 3000;

void setup()
{
  // Initialize serial communication with Arduino and the Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  // Initialize serial communication with Arduino and the SIM800L module
  Serial2.begin(9600);

  // while (!Serial2.available())
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  // gsm_send_serial("AT", await);

  Serial.println("ESP32+SIM800L AT CMD Test");

  location location;

  location.lat = -7.407788;
  location.lon = 109.253659;

  configGprs();

  httpPost(toJson(location));

  Serial.print("Finished in ");
  Serial.println(millis());
}

void loop()
{
}

void configGprs()
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
  gsm_send_serial("AT+HTTPACTION=1", 3000);
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
  gsm_send_serial("AT+HTTPPARA=\"URL\",\"" + serverName + getPath + "\"", await);
  gsm_send_serial("AT+HTTPACTION=0", await);
  gsm_send_serial("AT+HTTPREAD", await);
  gsm_send_serial("AT+HTTPTERM", await);
  gsm_send_serial("AT+SAPBR=0,1", await);
}

String toJson(location location)
{
  return "{\"latitude\":" + String(location.lat, 6U) + ",\"longitude\":" + String(location.lon, 6U) + "}";
}

byte gsm_send_serial(String command, int await)
{
  byte response;

  Serial.println("Send ->: " + command);
  Serial2.println(command);
  long wtimer = millis();
  while (wtimer + await > millis())
  {
    while (Serial2.available())
    {
      response = Serial2.read();

      Serial.write(response);
    }
  }

  Serial.println();

  return response;
}