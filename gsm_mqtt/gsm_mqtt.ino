#include <Arduino.h>
#include "SSLClient.h"
#include <PubSubClient.h>

// This is the root CA certificate
const char root_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Configure the SIM800L modem
#define MODEM_UART_BAUD 115200
#define MODEM_RST 5
#define LED_PIN 13
#define IP5306_ADDR 0x75
#define IP5306_REG_SYS_CTL0 0x00

// Configure the serial console for debug and the modem
#define serialMonitor Serial // Set serial for debug console (to the Serial Monitor)
#define serialModem Serial2  // Set serial for AT commands (to the SIM800 module)

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800   // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#include <TinyGsmClient.h>

// Your GPRS credentials (leave empty, if missing)
const char apn[] = "Telkomsel";    // Your APN
const char gprs_user[] = "wap";    // User
const char gprs_pass[] = "wap123"; // Password
const char simPIN[] = "";          // SIM card PIN code, if any

// Configure the MQTT broker
#define MQTT_BROKER "5dda2b51fb16459eb2e2de9ea499b546.s1.eu.hivemq.cloud" // e.g. a2l1dde17xdr1y-ats.iot.eu-west-1.amazonaws.com
#define MQTT_PORT 8883
#define MQTT_USERNAME "admin"
#define MQTT_PASSWORD "Admin123"
#define MQTT_CLIENT_ID "ESP_GPS_TRACKER"
#define MQTT_TIMEOUT 15000 // Set timeout for SSL connection (in ms)

#define SUB_ENGINE "saujanashafi/esp/engine"
#define SUB_START "saujanashafi/esp/start"
#define PUB_GPS "saujanashafi/esp/gps"

// Create the mqtt stack
TinyGsm modem(serialModem);
TinyGsmClient tcpClient(modem);
SSLClient ssl_client(&tcpClient);
PubSubClient mqttClient(ssl_client);

// Function prototypes

void nbConnect(void);
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void setupModem();
void setMQTTClientParams();
bool getIpAddress(const char *domain, char *ipAddress, size_t size);

// To use this example create your thing in AWS and get the certs downloaded. Be sure to update the ca_cert.h
// with the root CA, client certificate, and private key. The root CA is the Amazon Root CA 1.
// The client certificate and private key are the ones you downloaded from AWS IoT Core.
// The client certificate and private key are in PEM format.
// The root CA is in PEM format as well.

void setup()
{
  serialMonitor.begin(115200);
  delay(100);

  // Set SIM module baud rate and UART pins
  serialModem.begin(115200);

  setupModem();

  nbConnect();

  // Set the MQTT client parameters
  setMQTTClientParams();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);
}

void loop()
{
  // We maintain connectivity with the broker
  if (!mqttClient.connected())
  {
    reconnect();
  }
  // We are listening to the events
  mqttClient.loop();
  delay(10000);
}

/**
 * @brief Set the MQTT client parameters.
 * Sets the CA certificate, client certificate, and private key for the MQTT client.
 * Sets the timeout for the SSL connection.
 */
void setMQTTClientParams(void)
{
  ssl_client.setCACert(root_ca);
  ssl_client.setTimeout(MQTT_TIMEOUT);
}

/**
 * @brief Connect to the network and wait for the network to be available.
 */
void nbConnect()
{
  unsigned long start = millis();
  log_i("Initializing modem...");
  while (!modem.init())
  {
    log_i("waiting....%s", String((millis() - start) / 1000).c_str());
  };
  start = millis();
  log_i("Waiting for network...");
  while (!modem.waitForNetwork())
  {
    log_i("waiting....%s", String((millis() - start) / 1000).c_str());
  }
  log_i("success");
}

/**
 * @brief Callback function for the MQTT client.
 *
 * @param topic
 * @param payload
 * @param length
 */
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/**
 * @brief Stay connected to the MQTT broker
 */
void reconnect()
{
  setMQTTClientParams();

  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("-----------------------------------connected-----------------------");
      mqttClient.publish(PUB_GPS, "Test publish");
      mqttClient.subscribe(SUB_ENGINE);
      mqttClient.subscribe(SUB_START);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println("...try again in 5 seconds");
      delay(5000);
    }
  }
}

/**
 * @brief Initialize the modem and connect to the network.
 *
 */
void setupModem()
{
  pinMode(MODEM_RST, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Reset pin high
  digitalWrite(MODEM_RST, HIGH);

  // Initialize the indicator as an output
  digitalWrite(LED_PIN, LOW);

  // Initialize modem
  serialMonitor.print("Initializing modem...");
  if (!modem.init())
  {
    serialMonitor.print(" fail... restarting modem...");
    setupModem();
    // Restart takes quite some time
    // Use modem.init() if you don't need the complete restart
    if (!modem.restart())
    {
      serialMonitor.println(" fail... even after restart");
      return;
    }
  }
  serialMonitor.println(" OK");

  // General information
  String name = modem.getModemName();
  Serial.println("Modem Name: " + name);
  String modem_info = modem.getModemInfo();
  Serial.println("Modem Info: " + modem_info);

  // Wait for network availability
  serialMonitor.print("Waiting for network...");
  if (!modem.waitForNetwork(240000L))
  {
    serialMonitor.println(" fail");
    delay(10000);
    return;
  }
  serialMonitor.println(" OK");

  // Connect to the GPRS network
  serialMonitor.print("Connecting to network...");
  if (!modem.isNetworkConnected())
  {
    serialMonitor.println(" fail");
    delay(10000);
    return;
  }
  serialMonitor.println(" OK");

  // Connect to APN
  serialMonitor.print(F("Connecting to APN: "));
  serialMonitor.print(apn);
  if (!modem.gprsConnect(apn))
  {
    serialMonitor.println(" fail");
    delay(10000);
    return;
  }
  digitalWrite(LED_PIN, HIGH);
  serialMonitor.println(" OK");

  // More info..
  serialMonitor.println("");
  String ccid = modem.getSimCCID();
  serialMonitor.println("CCID: " + ccid);
  String imei = modem.getIMEI();
  serialMonitor.println("IMEI: " + imei);
  String cop = modem.getOperator();
  serialMonitor.println("Operator: " + cop);
  IPAddress local = modem.localIP();
  serialMonitor.println("Local IP: " + String(local));
  int csq = modem.getSignalQuality();
  serialMonitor.println("Signal quality: " + String(csq));
  // Check for IP address
  modem.sendAT(GF("+CIFSR")); // Get local IP address

  char ipAddress[32];

  if (getIpAddress(MQTT_BROKER, ipAddress, sizeof(MQTT_BROKER)))
  {
    serialMonitor.print("IP Address for ");
    serialMonitor.print(MQTT_BROKER);
    serialMonitor.print(" is ");
    serialMonitor.println(ipAddress);
  }
  else
  {
    serialMonitor.println("Failed to retrieve IP address");
  }

  // If successful, close the TCP connection and proceed
  modem.sendAT(GF("+CIPCLOSE"));
  modem.waitResponse();

  serialMonitor.println("Modem initialized and server reachable.");
}

/**
 * @brief Get the Ip Address object
 * This is akin to the ping command in Linux,
 * it is good to check if the server is reachable.
 *
 * @param domain
 * @param ipAddress
 * @param size
 * @return true
 * @return false
 */
bool getIpAddress(const char *domain, char *ipAddress, size_t size)
{
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "+CDNSGIP=\"%s\"", domain);
  modem.sendAT(cmd);

  char response[128];
  unsigned long start = millis();
  bool gotResponse = false;

  while (millis() - start < 10000L)
  {
    if (modem.stream.available())
    {
      size_t len = modem.stream.readBytesUntil('\n', response, sizeof(response) - 1);
      response[len] = '\0'; // Null-terminate the string
      if (strstr(response, "+CDNSGIP:"))
      {
        gotResponse = true;
        break;
      }
    }
  }

  if (gotResponse)
  {
    // Assuming the response format is +CDNSGIP: 1,"domain","IP1","IP2"
    char *startChar = strchr(response, '"');
    if (startChar != NULL)
    {
      startChar = strchr(startChar + 1, '"');
      if (startChar != NULL)
      {
        startChar = strchr(startChar + 1, '"');
        if (startChar != NULL)
        {
          char *endChar = strchr(startChar + 1, '"');
          if (endChar != NULL && endChar > startChar && (size_t)(endChar - startChar - 1) < size)
          {
            strncpy(ipAddress, startChar + 1, endChar - startChar - 1);
            ipAddress[endChar - startChar - 1] = '\0'; // Ensure null-termination
            return true;
          }
        }
      }
    }
  }

  ipAddress[0] = '\0'; // Ensure the buffer is set to empty string if no IP found
  return false;
}