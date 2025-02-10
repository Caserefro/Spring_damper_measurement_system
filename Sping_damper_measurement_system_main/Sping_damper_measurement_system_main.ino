#include "functions.h"

//*****************************************************************************
// Interrupt Service Routines (ISRs)
// ISR for the timer
/*void IRAM_ATTR onTimer() {
  microsCount++;  // Increment microsecond count
}*/

void IRAM_ATTR FIRST_IMU_ISR_FLAG() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyFromISR(Read_Data, 0, eNoAction, &xHigherPriorityTaskWoken);  // Send notification to task
  if (xHigherPriorityTaskWoken == pdTRUE) {
    portYIELD_FROM_ISR();  // If a higher-priority task is unblocked by the notification, yield to it
  }
}

//*****************************************************************************
// Tasks
//gets notified by FIRST_IMU_ISR_FLAG, and reads the value and sets the timestamp (obtained from HW timer(nanoseconds since startup))
void ReadData(void *parameters) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for notification indefinitely
    xyzFloat gValue = FIRST_MPU6500.getGValues();
    Sensordata.zGvalue = gValue.z;
    lastread = millis() - lastread;
    Sensordata.Timestamp = lastread;
    xQueueSend(msg_queue, (void *)&Sensordata, 0) == pdTRUE;
    lastread = millis();
  }
}

//checks for pairs of (data, Timestamp) in queue, if so create a string that will be published to ws
void PublishData(void *parameters) {
  while (1) {
    if (xQueueReceive(msg_queue, (void *)&Sensordata, 7) == pdTRUE) { // 7 bc 7ms takes for one read to be done. (could be less, not tested.)
      wspackage += (String(Sensordata.zGvalue) + "," + String(Sensordata.Timestamp) + ",");
      if (wspackage.length() > 300) {
        ws.textAll(wspackage);
        Serial.println(wspackage);
        wspackage = "";
      }
    }
  }
}



void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Wire.begin();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  init_FIRST_MPU6500();
  enableINTmpu6500(csPinMPU1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  attachInterrupt(FIRST_IMU_ISR_PIN, FIRST_IMU_ISR_FLAG, RISING);

  // Create message queue before it is used
  msg_queue = xQueueCreate(MSG_QUEUE_LEN, sizeof(Sensordata));

  xTaskCreate(ReadData,
              "Read data",
              1024,
              NULL,
              1,
              &Read_Data);

  xTaskCreate(PublishData,
              "Publish data",
              2000,
              NULL,
              1,
              &Publish_data);


  /*// Set the timer period in microseconds
  timer = timerBegin(0, 80, true);  // 80 prescaler, true count up
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1, true);  // 1 microsecond period
  timerAlarmEnable(timer);*/

  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
}
