#include <WiFi.h>
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

// Configure the serial console for debug and the modem
#define serialMonitor Serial // Set serial for debug console (to the Serial Monitor)

// Configure the MQTT broker
#define MQTT_BROKER "5dda2b51fb16459eb2e2de9ea499b546.s1.eu.hivemq.cloud" // e.g. a2l1dde17xdr1y-ats.iot.eu-west-1.amazonaws.com
#define MQTT_PORT 8883
#define MQTT_USERNAME "admin"
#define MQTT_PASSWORD "Admin123"
#define MQTT_CLIENT_ID "ESP_GPS_TRACKER"
#define MQTT_TIMEOUT 15000 // Set timeout for SSL connection (in ms)

// Setup wifi
#define WIFI_AP "OPPO A92"
#define WIFI_PASS "okayyyyy"

// Create the mqtt stack
WiFiClient wifiClient;
SSLClient ssl_client(&wifiClient);
PubSubClient mqttClient(ssl_client);

// Function prototypes
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void setupWifi();
void setMQTTClientParams();

// To use this example create your thing in AWS and get the certs downloaded. Be sure to update the ca_cert.h
// with the root CA, client certificate, and private key. The root CA is the Amazon Root CA 1.
// The client certificate and private key are the ones you downloaded from AWS IoT Core.
// The client certificate and private key are in PEM format.
// The root CA is in PEM format as well.

void setup()
{
  serialMonitor.begin(115200);

  serialMonitor.println("Starting....");

  delay(100);

  setupWifi();

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
 * @brief Callback function for the MQTT client.
 *
 * @param topic
 * @param payload
 * @param length
 */
void callback(char *topic, byte *payload, unsigned int length)
{
  serialMonitor.print("Message arrived [");
  serialMonitor.print(topic);
  serialMonitor.print("] ");
  for (int i = 0; i < length; i++)
  {
    serialMonitor.print((char)payload[i]);
  }
  serialMonitor.println();
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
    serialMonitor.println("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
      serialMonitor.println("-----------------------------------connected-----------------------");
      mqttClient.subscribe("esp/test");
    }
    else
    {
      serialMonitor.print("failed, rc=");
      serialMonitor.print(mqttClient.state());
      serialMonitor.println("...try again in 5 seconds");
      delay(5000);
    }
  }
}

/**
 * @brief Initialize the modem and connect to the network.
 *
 */
void setupWifi()
{
  serialMonitor.println("Connecting to WiFi...");
  int attempts = 0;

  WiFi.begin(WIFI_AP, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    // spinner();
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    serialMonitor.println("\nFailed to connect to WiFi");
    delay(3000);
  }
  else
  {
    serialMonitor.println("\nConnected to WiFi");
    delay(3000);
  }
}
