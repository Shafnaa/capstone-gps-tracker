// Select your modem:
#define TINY_GSM_MODEM_SIM800

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
#define SerialAT Serial2

// Path to the certificate file on the SIM800l
#define CERT_FILE "C:\\User\\certificate.crt"

// set GSM PIN, if any
#define GSM_PIN ""

// Root CA Certificate
// Load DigiCert Global Root G2, which is used by EMQX Public Broker: broker.emqx.io
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl
MrY=
-----END CERTIFICATE-----
)EOF";

// Load DigiCert Global Root CA ca_cert, which is used by EMQX Cloud Serverless Deployment
/*
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
*/

// Your GPRS credentials, if any
const char apn[] = "Telkomsel";
const char gprsUser[] = "wap";
const char gprsPass[] = "wap123";

// MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic = "emqx/esp32";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 8883;

#include <TinyGsmClient.h>
#include <PubSubClient.h>

// SIM800l and MQTT client initialization
TinyGsm modem(SerialAT);
TinyGsmClientSecure client(modem);
PubSubClient mqtt_client(client);

uint32_t lastReconnectAttempt = 0;

// Function Declarations
void setupModem();

void setupCert();

boolean connectToMQTT();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup()
{
  SerialMon.begin(115200);

  // Set GSM module baud rate
  SerialAT.begin(115200);

  SerialMon.println("Initializing modem...");
  modem.init();

  setupCert();

  setupModem();

  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setKeepAlive(60);
  mqtt_client.setCallback(mqttCallback);
}

void setupCert()
{
  modem.sendAT(GF("+FSCREATE=" CERT_FILE));
  if (modem.waitResponse() != 1)
    return;

  const int cert_size = sizeof(ca_cert);

  modem.sendAT(GF("+FSWRITE=" CERT_FILE ",0,"), cert_size, GF(",10"));

  if (modem.waitResponse(GF(">")) != 1)
  {
    return;
  }

  for (int i = 0; i < cert_size; i++)
  {
    char c = pgm_read_byte(&ca_cert[i]);
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
  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  modem.sendAT("+SSLOPT=1,1");
  int rsp = modem.waitResponse();
  if (rsp != 1)
  {
    SerialMon.printf("modem +SSLOPT=1,1 failed");
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

boolean connectToMQTT()
{
  String client_id = "esp32-client";

  SerialMon.print("Connecting to ");
  SerialMon.print(mqtt_broker);
  SerialMon.printf(" as %s", client_id.c_str());

  // Connect to MQTT Broker
  boolean status = mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password);

  if (status == false)
  {
    SerialMon.println(" fail");
    return false;
  }

  SerialMon.println(" success");
  mqtt_client.subscribe(mqtt_topic);
  mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP32 ^^"); // Publish message upon connection

  return mqtt_client.connected();
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  SerialMon.print("Message received on topic: ");
  SerialMon.println(topic);
  SerialMon.print("Message: ");
  for (unsigned int i = 0; i < length; i++)
  {
    SerialMon.print((char)payload[i]);
  }
  SerialMon.println("\n-----------------------");
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

  if (!mqtt_client.connected())
  {
    SerialMon.println("=== MQTT NOT CONNECTED ===");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L)
    {
      lastReconnectAttempt = t;
      if (connectToMQTT())
      {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }

  mqtt_client.loop();
}