/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/
TaskHandle_t Task1;

void setup()
{
  Serial.begin(115200);

  xTaskCreatePinnedToCore(
      taskOneCode, /* Task function. */
      "Task1",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &Task1,      /* Task handle to keep track of created task */
      0);          /* pin task to core 0 */

  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  // xTaskCreatePinnedToCore(
  //     taskTwoCode, /* Task function. */
  //     "Task2",     /* name of task. */
  //     10000,       /* Stack size of task */
  //     NULL,        /* parameter of the task */
  //     1,           /* priority of the task */
  //     &Task2,      /* Task handle to keep track of created task */
  //     1);          /* pin task to core 1 */

  Serial.print("Task 2 running on core: ");
  Serial.println(xPortGetCoreID());

  delay(500);
}

void loop()
{
  Serial.println("Task 2");
  delay(5000);
}

void taskOneCode(void *pvParameters)
{
  Serial.print("Task 1 running on core: ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    Serial.println("Task 1");
    delay(10000);
  }
}

void taskTwoCode(void *pvParameters)
{
  Serial.print("Task 2 running on core: ");
  Serial.println(xPortGetCoreID());

  for (;;)
  {
    Serial.println("Task 2");
    delay(5000);
  }
}