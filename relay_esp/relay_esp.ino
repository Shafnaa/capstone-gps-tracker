#define relay 15

bool state = false;

void setup()
{
  Serial.begin(115200);

  pinMode(relay, OUTPUT);
}

void loop()
{
  Serial.println(state);

  digitalWrite(relay, state);

  state = !state;

  delay(10000);
}