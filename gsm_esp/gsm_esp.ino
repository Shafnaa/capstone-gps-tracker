String apn = "TSEL-SNS";

String serverName = "https://ycppywtjigaamvdfjfge.supabase.co";
String pathName = "/rest/v1/tracks";
String apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InljcHB5d3RqaWdhYW12ZGZqZmdlIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MTYzMDMxOTEsImV4cCI6MjAzMTg3OTE5MX0.T-WZ2m4MEMV_7aHcv0R5Md9Iixj1gQU-CmSPRaYrKuA";

struct location
{
  double lat;
  double lon;
};

void setup()
{
  // Initialize serial communication with Arduino and the Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  // Initialize serial communication with Arduino and the SIM800L module
  Serial2.begin(9600);

  while (!Serial2.available())
  {
    delay(500);
    Serial.print(".");
  }

  gsm_send_serial("AT");

  Serial.println("ESP32+SIM800L AT CMD Test");

  location location;

  location.lat = -7.407788;
  location.lon = 109.253659;

  configGprs();

  httpPost(toJson(location));

  Serial.println("Finished...");

  gsm_send_serial("ATD+6285890926500;"); // Change ZZ with the country code and xxxxxxxxxxx with the phone number to dial

  delay(20000); // Wait for 20 seconds...

  gsm_send_serial("ATH");
}

void loop()
{
}

void configGprs()
{
  Serial.println("Configuring GPRS...");
  gsm_send_serial("AT+SAPBR=3,1,Contype,\"GPRS\"");
  gsm_send_serial("AT+SAPBR=3,1,APN,\"" + apn + "\"");
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

String toJson(location location)
{
  return "{\"lat\":" + String(location.lat, 6U) + ",\"lon\":" + String(location.lon, 6U) + "}";
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