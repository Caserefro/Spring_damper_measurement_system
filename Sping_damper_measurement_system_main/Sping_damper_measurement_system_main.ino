#include "functions.h"

#include <esp32-hal-timer.h>

// Declare global variables
hw_timer_t *timer = NULL;
volatile uint64_t microsCount = 0;

struct SensorDatadef {
  float zGvalue;
  long long int Timestamp;
} Sensordata;

//static hw_timer_t *timer = NULL;
static TaskHandle_t Read_Data = NULL;
static TaskHandle_t Publish_data = NULL;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
static QueueHandle_t msg_queue;
enum { MSG_QUEUE_LEN = 10 };  // Number of slots in message queue
String wspackage = " ";


//*****************************************************************************
// Interrupt Service Routines (ISRs)
void IRAM_ATTR FIRST_IMU_ISR_FLAG() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyFromISR(Read_Data, 0, eNoAction, &xHigherPriorityTaskWoken);  // Send notification to task
  if (xHigherPriorityTaskWoken == pdTRUE) {
    portYIELD_FROM_ISR();  // If a higher-priority task is unblocked by the notification, yield to it
  }
}

// ISR for the timer
void IRAM_ATTR onTimer() {
  microsCount++;  // Increment microsecond count
  aux = millis();
}

//*****************************************************************************
// Tasks
//gets notified by FIRST_IMU_ISR_FLAG, and reads the value and sets the timestamp (obtained from HW timer(nanoseconds since startup))
void ReadData(void *parameters) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for notification indefinitely
    xyzFloat gValue = FIRST_MPU6500.getGValues();
    Sensordata.zGvalue = gValue.z;
    Sensordata.Timestamp = millis();
    xQueueSend(msg_queue, (void *)&Sensordata, 0) == pdTRUE;
  }
}

//checks if 5 pairs of (data, Timestamp) are in queue, if so, publish to ws
void PublishData(void *parameters) {
  while (1) {
    if (xQueueReceive(msg_queue, (void *)&Sensordata, 7) == pdTRUE) {
      wspackage += (String(Sensordata.zGvalue) + "," + String(Sensordata.Timestamp) + ",");
      if (wspackage.length() > 500) {
        
        
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
  init_FIRST_MPU6500();
  enableINTmpu6500(csPinMPU1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  attachInterrupt(FIRST_IMU_ISR_PIN, FIRST_IMU_ISR_FLAG, RISING);

  // Create message queue before it is used
  msg_queue = xQueueCreate(MSG_QUEUE_LEN, sizeof(Sensordata));

  // Start task to handle command line interface events. Let's set it at a
  // higher priority but only run it once every 20 ms.
  xTaskCreate(ReadData,
              "Read data",
              1024,
              NULL,
              1,
              &Read_Data);

  // Start task to calculate average. Save handle for use with notifications.
  xTaskCreate(PublishData,
              "Publish data",
              1024,
              NULL,
              1,
              &Publish_data);


  /* // Set the timer period in microseconds
  timer = timerBegin(0, 80, true);  // 80 prescaler, true count up
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1, true);  // 1 microsecond period
  timerAlarmEnable(timer);
  timerStart(timer);
*/
  // Delete "setup and loop" task
  vTaskDelete(NULL);
}

void loop() {
  // Execution should never get here
}
/*
void loop() {
  if (FIRST_IMUReadingAvailable && measures > 0) {
    // Serial.println(millis() - aux);
    //aux = millis();
    xyzFloat gValue = FIRST_MPU6500.getGValues();
    Serial.print("IMU 1 reading ");
    Serial.println(measures);
    Serial.println(gValue.z);
    // Serial.print("Resultant g: ");
    // Serial.println(resultantG);
    FIRST_IMUReadingAvailable = false;
    measures--;
  }
  if (measures == 0) {
    Serial.print(millis() - aux);
    delay(1000000);
  }
}
*/