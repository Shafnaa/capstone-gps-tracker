

void setup()
{
  // Initialize serial communication with Arduino and the Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  // Initialize serial communication with Arduino and the SIM800L module
  Serial2.begin(9600);

  gsm_send_serial("AT");

  gsm_send_serial("ATD+6285890926500;"); // Change ZZ with the country code and xxxxxxxxxxx with the phone number to dial

  delay(20000); // Wait for 20 seconds...

  gsm_send_serial("ATH");
}

void loop()
{
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