#include <SIM800L.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

#define SIM800_RST_PIN 5

struct location
{
  double lat;
  double lon;
};

// The TinyGPSPlus object
TinyGPSPlus gps;

// Updating location concurrently
TaskHandle_t gpsTask;

// Assigning GPS to serial port 1
HardwareSerial SerialPort(1);

const char apn[] = "TSEL-SNS";

const char postPath[] = "http://sherlockin.000webhostapp.com/api.php";
const char getPath[] = "http://sherlockin.000webhostapp.com/data.php";
const char header[] = "Authorization: Bearer kepston";
const char contentType[] = "application/json";

SIM800L *sim800l;

location currentLocation, oldLocation;

void setup()
{
  // Initialize serial communication with Arduino and the Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  // Initialize serial communication with Arduino and the SIM800L module
  Serial2.begin(9600);
  // Initialize serial communication with Arduino and the SIM800L module
  SerialPort.begin(9600, SERIAL_8N1, 4, 2);

  xTaskCreatePinnedToCore(
      updateLocation, /* Task function. */
      "GPS Task",     /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &gpsTask,       /* Task handle to keep track of created task */
      0);

  // Initialize SIM800L driver with an internal buffer of 200 bytes and a reception buffer of 512 bytes, debug disabled
  sim800l = new SIM800L((Stream *)&Serial2, SIM800_RST_PIN, 200, 512);

  Serial.println("ESP32+SIM800L AT CMD Test");

  setupModule();

  while (currentLocation.lat == NULL && currentLocation.lon == NULL)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\n");
}

void loop()
{
  // Establish GPRS connectivity (5 trials)
  bool connected = checkConnection();

  // Check if connected, if not reset the module and setup the config again
  if (connected)
  {
    Serial.print(F("GPRS connected with IP "));
    Serial.println(sim800l->getIP());
  }
  else
  {
    Serial.println(F("GPRS not connected !"));
    Serial.println(F("Reset the module."));
    sim800l->reset();
    setupModule();
    return;
  }

  httpPost();

  // Establish GPRS connectivity (5 trials)
  connected = checkConnection();

  // Check if connected, if not reset the module and setup the config again
  if (connected)
  {
    Serial.print(F("GPRS connected with IP "));
    Serial.println(sim800l->getIP());
  }
  else
  {
    Serial.println(F("GPRS not connected !"));
    Serial.println(F("Reset the module."));
    sim800l->reset();
    setupModule();
    return;
  }

  httpGet();
}

void setupModule()
{
  // Wait until the module is ready to accept AT commands
  while (!sim800l->isReady())
  {
    Serial.println(F("Problem to initialize AT command, retry in 1 sec"));
    delay(1000);
  }
  Serial.println(F("Setup Complete!"));

  // Wait for the GSM signal
  uint8_t signal = sim800l->getSignal();
  while (signal <= 0)
  {
    delay(1000);
    signal = sim800l->getSignal();
  }
  Serial.print(F("Signal OK (strenght: "));
  Serial.print(signal);
  Serial.println(F(")"));
  delay(1000);

  // Wait for operator network registration (national or roaming network)
  NetworkRegistration network = sim800l->getRegistrationStatus();
  while (network != REGISTERED_HOME && network != REGISTERED_ROAMING)
  {
    delay(1000);
    network = sim800l->getRegistrationStatus();
  }
  Serial.println(F("Network registration OK"));
  delay(1000);

  // Setup APN for GPRS configuration
  bool success = sim800l->setupGPRS(apn);
  while (!success)
  {
    success = sim800l->setupGPRS(apn);
    delay(5000);
  }
  Serial.println(F("GPRS config OK"));
}

void httpPost()
{
  Serial.println(F("Start HTTP POST..."));

  String payload = "{\"latitude\":" + String(currentLocation.lat, 6U) + ",\"longitude\":" + String(currentLocation.lon, 6U) + "}";

  // Do HTTP POST communication with 10s for the timeout (read and write)
  uint16_t rc = sim800l->doPost(postPath, contentType, payload.c_str(), 10000, 10000);
  if (rc == 200)
  {
    // Success, output the data received on the serial
    Serial.print(F("HTTP POST successful ("));
    Serial.print(sim800l->getDataSizeReceived());
    Serial.println(F(" bytes)"));
    Serial.print(F("Received : "));
    Serial.println(sim800l->getDataReceived());
  }
  else
  {
    // Failed...
    Serial.print(F("HTTP POST error "));
    Serial.println(rc);
  }
}

void httpGet()
{
  Serial.println(F("Start HTTP GET..."));

  // Do HTTP GET communication with 10s for the timeout (read)
  uint16_t rc = sim800l->doGet(getPath, "Authorization: Bearer kepston", 10000);
  if (rc == 200)
  {
    // Success, output the data received on the serial
    Serial.print(F("HTTP GET successful ("));
    Serial.print(sim800l->getDataSizeReceived());
    Serial.println(F(" bytes)"));
    Serial.print(F("Received : "));
    Serial.println(sim800l->getDataReceived());
  }
  else
  {
    // Failed...
    Serial.print(F("HTTP GET error "));
    Serial.println(rc);
  }
}

void toLowPowerMode()
{
  // Go into low power mode
  bool lowPowerMode = sim800l->setPowerMode(MINIMUM);
  if (lowPowerMode)
  {
    Serial.println(F("Module in low power mode"));
  }
  else
  {
    Serial.println(F("Failed to switch module to low power mode"));
  }
}

bool checkConnection()
{
  // Establish GPRS connectivity (5 trials)
  bool connected = false;
  for (uint8_t i = 0; i < 5 && !connected; i++)
  {
    delay(1000);
    connected = sim800l->connectGPRS();
  }

  return connected;
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