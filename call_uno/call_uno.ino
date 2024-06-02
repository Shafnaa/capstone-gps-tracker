#include <SoftwareSerial.h>

// Create software serial object to communicate with SIM800L
SoftwareSerial sim800l(11, 10); // SIM800L Tx & Rx is connected to Arduino #11 & #10

void setup()
{
  // Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  // Begin serial communication with Arduino and SIM800L
  sim800l.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  sim800l.println("AT"); // Once the handshake test is successful, i t will back to OK
  updateSerial();

  String SIM_PIN_CODE = String("1234");
  sim800l.print("AT+CPIN=");
  sim800l.println(SIM_PIN_CODE);
  updateSerial();

  sim800l.println("ATD+ +6282328826221;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(20000);           // wait for 20 seconds...
  sim800l.println("ATH"); // hang up
  updateSerial();
}

void loop()
{
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    sim800l.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (sim800l.available())
  {
    Serial.write(sim800l.read()); // Forward what Software Serial received to Serial Port
  }
}