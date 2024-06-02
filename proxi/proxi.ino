#define proximity 8
#define led 13

void setup()
{
  Serial.begin(9600);

  pinMode(proximity, INPUT);
  pinMode(led, OUTPUT);
}

void loop()
{
  while (!digitalRead(proximity))
  {
    Serial.println("\nObject Detected");
    digitalWrite(led, HIGH);
    // delay(500);
  }
  Serial.print(".");
  digitalWrite(led, LOW);
}