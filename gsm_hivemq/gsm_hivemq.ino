// #define DUMP_AT_COMMANDS

// Select your modem:
#define TINY_GSM_MODEM_SIM800

// #define TINY_GSM_SSL_CLIENT_AUTHENTICATION

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
#define SerialAT Serial2

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "Telkomsel";
const char gprsUser[] = "wap";
const char gprsPass[] = "wap123";

#include "certificate.h"

#define CERT_FILE "C:\\User\\certificate.crt"

// MQTT details
const char *broker = "89519cd9d3fd4ccfb9307a02cc620b42.s1.eu.hivemq.cloud";
const char *mqttUser = "admin";
const char *mqttPass = "Admin123";

const char *topicEngine = "saujanashafi/esp/engine";
const char *topicEngineStatus = "saujanashafi/esp/engine/status";
const char *topicStart = "saujanashafi/esp/start";
const char *topicStartStatus = "saujanashafi/esp/start/status";
const char *topicGPS = "saujanashafi/esp/gps";

#include <TinyGsmClient.h>
#include <SSLClient.h>
#include <PubSubClient.h>
#include <TinyGPSPlus.h>

// The TinyGsm modem & MQTT Client
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
SSLClient sslClient(&client);
PubSubClient mqtt(sslClient);

// The TinyGPSPlus object
TinyGPSPlus gps;

// Updating location concurrently
TaskHandle_t gpsTask;

// Assigning GPS to serial port 1
HardwareSerial SerialPort(1);

struct location
{
  double lat;
  double lng;
};

#define RST_GSM 5

#define RELAY_PIN 15
int engineStatus = LOW;
int startStatus = LOW;

uint32_t lastReconnectAttempt = 0;

location currentLocation;

String msg, lastMsg;

// Last message sent
long lastSent = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();

  if (len == 1)
  {
    // Only proceed if incoming message's topic matches
    if (String(topic) == topicEngine)
    {
      if ((char)payload[0] == '1')
        engineStatus = HIGH;
      else if ((char)payload[0] == '0')
        engineStatus = LOW;
      digitalWrite(RELAY_PIN, engineStatus);
      mqtt.publish(topicEngineStatus, engineStatus ? "1" : "0", true);
      SerialMon.println(engineStatus ? "Engine on..." : "Engine off...");
    }

    // Only proceed if incoming message's topic matches
    if (String(topic) == topicStart)
    {
      if ((char)payload[0] == '1')
        startStatus = HIGH;
      else if ((char)payload[0] == '0')
        startStatus = LOW;
      mqtt.publish(topicStartStatus, startStatus ? "1" : "0", true);
      SerialMon.println(startStatus ? "Start tracking..." : "Stop tracking...");
    }
  }
}

boolean mqttConnect()
{
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker
  boolean status = mqtt.connect("SaujanaShafiGPS", mqttUser, mqttPass);

  if (status == false)
  {
    SerialMon.println(" fail");
    return false;
  }

  SerialMon.println(" success");
  mqtt.subscribe(topicEngine);
  mqtt.subscribe(topicStart);

  return mqtt.connected();
}

void setupCert()
{
  modem.sendAT(GF("+FSCREATE=" CERT_FILE));
  if (modem.waitResponse() != 1)
    return;

  const int cert_size = sizeof(cert);

  modem.sendAT(GF("+FSWRITE=" CERT_FILE ",0,"), cert_size, GF(",10"));

  if (modem.waitResponse(GF(">")) != 1)
  {
    return;
  }

  for (int i = 0; i < cert_size; i++)
  {
    char c = pgm_read_byte(&cert[i]);
    modem.stream.write(c);
  }

  modem.stream.write(GSM_NL);
  modem.stream.flush();

  if (modem.waitResponse(2000) != 1)
    return;

  modem.sendAT(GF("+SSLSETCERT=\"" CERT_FILE "\""));
  if (modem.waitResponse() != 1)
    return;
  if (modem.waitResponse(5000L, GF(GSM_NL "+SSLSETCERT:")) != 1)
    return;
  const int retCode = modem.stream.readStringUntil('\n').toInt();

  SerialMon.println();
  SerialMon.println();
  SerialMon.println(F("****************************"));
  SerialMon.print(F("Setting Certificate: "));
  SerialMon.println((0 == retCode) ? "OK" : "FAILED");
  SerialMon.println(F("****************************"));
}

void setupModem()
{
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  // modem.restart();
  modem.init();

  setupCert();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  modem.sendAT("+SSLOPT=1,1");
  int rsp = modem.waitResponse();
  if (rsp != 1)
  {
    Serial.printf("modem +SSLOPT=1,1 failed");
  }

  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3)
  {
    modem.simUnlock(GSM_PIN);
  }

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected())
  {
    SerialMon.println("Network connected");
  }

  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected())
  {
    SerialMon.println("GPRS connected");
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
          currentLocation.lng = gps.location.lng();
        }
        else
          continue;
      }
    };

    delay(500);
  }
}

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  pinMode(RELAY_PIN, OUTPUT);

  currentLocation.lat = -7.989723;
  currentLocation.lng = 103.628453;

  // !!!!!!!!!!!
  // Set your reset, enable, power pins here
  // !!!!!!!!!!!
  pinMode(RST_GSM, OUTPUT);
  digitalWrite(RST_GSM, HIGH);
  modem.init();

  SerialMon.println("Wait...");

  // Set GSM module baud rate
  SerialAT.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 4, 2);

  xTaskCreatePinnedToCore(
      updateLocation, /* Task function. */
      "GPS Task",     /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &gpsTask,       /* Task handle to keep track of created task */
      0);

  sslClient.setCACert(cert);

  delay(6000);

  setupModem();

  // MQTT Broker setup
  mqtt.setServer(broker, 8883);
  mqtt.setCallback(mqttCallback);

  while (currentLocation.lat == NULL && currentLocation.lng == NULL)
  {
    SerialMon.print(".");

    delay(500);
  }

  SerialMon.println();
}

void loop()
{
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected())
  {
    SerialMon.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true))
    {
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected())
    {
      SerialMon.println("Network re-connected");
    }

    // and make sure GPRS/EPS is still connected
    if (!modem.isGprsConnected())
    {
      SerialMon.println("GPRS disconnected!");
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass))
      {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected())
      {
        SerialMon.println("GPRS reconnected");
      }
    }
  }

  // client.connect(broker, 8883);

  if (!mqtt.connected())
  {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L)
    {
      lastReconnectAttempt = t;
      if (mqttConnect())
      {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  long now = millis();
  if ((now - lastSent > 30000) && startStatus)
  {
    msg = String(currentLocation.lat, 6U) + "," + String(currentLocation.lng, 6U);

    if (msg != lastMsg)
    {
      lastSent = now;

      mqtt.publish(topicGPS, msg.c_str());
      SerialMon.println("Published message: " + msg);
      lastMsg = msg;
    }
  }

  mqtt.loop();
}