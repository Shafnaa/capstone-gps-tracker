#include <TinyGPSPlus.h>

// The TinyGPSPlus object

TinyGPSPlus gps;

void setup()
{

  Serial.begin(115200);

  Serial2.begin(9600);

  delay(3000);
  Serial.println("Starting...");
}

void loop()
{

  // updateSerial();

  while (Serial2.available() > 0)
  {
    if (gps.encode(Serial2.read()))

      displayInfo();
    // delay(5000);
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));

    while (true)
      ;
  }
}

void displayInfo()

{

  Serial.print(F("Location: "));

  if (gps.location.isValid())
  {

    Serial.print("Lat: ");

    Serial.print(gps.location.lat(), 6);

    Serial.print(F(","));

    Serial.print("\tLng: ");

    Serial.print(gps.location.lng(), 6);

    Serial.println();
  }

  else

  {

    Serial.println(F("INVALID"));
  }
}

void updateSerial()

{

  delay(500);

  while (Serial.available())

  {

    Serial2.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }

  while (Serial2.available())

  {

    Serial.write(Serial2.read()); // Forward what Software Serial received to Serial Port
  }
}
