// Select your modem:
#define TINY_GSM_MODEM_SIM800 // Modem is SIM800L

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands
#define SerialAT Serial2
// Set serial for GPS communication
#define SerialGPS SerialPort

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "TSEL-SNS"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "";
const char gprsPass[] = "";

// SIM card PIN (leave empty, if not defined)
const char simPIN[] = "";

// MQTT details
const char *broker = "5dda2b51fb16459eb2e2de9ea499b546.s1.eu.hivemq.cloud"; // Public IP address or domain name
const char *mqttUsername = "admin";                                      // MQTT username
const char *mqttPassword = "Admin123";                               // MQTT password
const int mqttPort = 8883;

const char *topicEngine = "esp/engine";
const char *topicGPS = "esp/gps";

// Define the serial console for debug prints, if needed
// #define DUMP_AT_COMMANDS

#include <TinyGsmClient.h>
#include <HardwareSerial.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

#include <PubSubClient.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>

TinyGsmClientSecure client(modem);
PubSubClient mqtt(client);
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

// TTGO T-Call pins
#define MODEM_RST 5
#define MODEM_TX 16
#define MODEM_RX 17

#define ENGINE_1 15

uint32_t lastReconnectAttempt = 0;

struct location
{
  double lat;
  double lng;
};

long lastMsg = 0;

location currentLocation;

void mqttCallback(char *topic, byte *message, unsigned int len)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < len; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp/output1, you check if the message is either "true" or "false".
  // Changes the output state according to the message
  Serial.print("Changing output to ");
  if (messageTemp == "true")
  {
    Serial.println("true");
    digitalWrite(ENGINE_1, HIGH);
  }
  else if (messageTemp == "false")
  {
    Serial.println("false");
    digitalWrite(ENGINE_1, LOW);
  }
}

boolean mqttConnect()
{
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  // Connect to MQTT Broker without username and password
  // boolean status = mqtt.connect("GsmClientN");

  // Or, if you want to authenticate MQTT:
  boolean status = mqtt.connect("GsmClientN", mqttUsername, mqttPassword);

  if (status == false)
  {
    SerialMon.println(" fail");
    ESP.restart();
    return false;
  }
  SerialMon.println(" success");
  mqtt.subscribe(topicEngine);

  return mqtt.connected();
}

void updateLocation(void *pvParameters)
{
  for (;;)
  {
    while (SerialGPS.available())
    {
      if (gps.encode(SerialGPS.read()))
      {
        if (gps.location.isValid())
        {
          currentLocation.lat = gps.location.lat();
          currentLocation.lng = gps.location.lng();

          delay(5000);
        }
        else
          continue;
      }
    }

    delay(500);
  };
}

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 4, 2);
  delay(10);

  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, HIGH);

  pinMode(ENGINE_1, OUTPUT);

  SerialMon.println("Wait...");

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3)
  {
    modem.simUnlock(GSM_PIN);
  }

  SerialMon.print("Connecting to APN: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.println(" fail");
    ESP.restart();
  }
  else
  {
    SerialMon.println(" OK");
  }

  if (modem.isGprsConnected())
  {
    SerialMon.println("GPRS connected");
  }

  // MQTT Broker setup
  mqtt.setServer(broker, mqttPort);
  mqtt.setCallback(mqttCallback);
}

void loop()
{
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

  if (now - lastMsg > 30000)
  {
    lastMsg = now;

    // Convert the value to a char array
    const char *locationString = "{\"lat\": -7.562684, \"lng\": 103.739423}";

    Serial.print("Temperature: ");
    Serial.println(locationString);
    mqtt.publish(topicGPS, locationString);
  }

  mqtt.loop();
}