#ifndef definitions.h
#define definitions_h
#include "Credentials.h"

// MPU THINGS --------------------------------------------------------------------------------
#include <MPU6500_WE.h>

//FIRST MPU 6500
const int csPinMPU1 = 16;  // Chip Select Pin
const int FIRST_IMU_ISR_PIN = 17;
//bool FIRST_IMUReadingAvailable = 0;
/*
//SECOND MPU 6500
const int csPinMPU2 = 16;  // Chip Select Pin
const int SECOND_IMU_ISR_PIN = 4;
bool SECOND_IMUReadingAvailable = 0;

//THIRD MPU 6500
const int csPinMPU3 = 17;  // Chip Select Pin
const int THIRD_IMU_ISR_PIN = 5;
bool THIRD_IMUReadingAvailable = 0;
*/
// Registers and configs used (Regs can be found in the MPU6500_WE library)
static uint8_t constexpr REGISTER_INT_PIN_CFG = 0x37;
static uint8_t constexpr REGISTER_INT_ENABLE = 0x38;
bool useSPI = true;  // SPI use flag

/* Constructors */
MPU6500_WE FIRST_MPU6500 = MPU6500_WE(&SPI, csPinMPU1, useSPI);
//MPU6500_WE SECOND_MPU6500 = MPU6500_WE(&SPI, csPinMPU2, useSPI);
//MPU6500_WE THIRD_MPU6500 = MPU6500_WE(&SPI, csPinMPU3, useSPI);

uint32_t lastread;
int measures = 100;
int measures2 = 100;
int measures3 = 100;

//WEBSOCKETS THINGS
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

unsigned long lastTime = 0;
const unsigned long interval = 1000;  // 1000 milliseconds = 1 second

// RTOS and timers
hw_timer_t *timer = NULL;
volatile uint64_t microsCount = 0;

struct SensorDatadef {
  float zGvalue;
  long int Timestamp;
} Sensordata;

//static hw_timer_t *timer = NULL;
static TaskHandle_t Read_Data = NULL;
static TaskHandle_t Publish_data = NULL;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
static QueueHandle_t msg_queue;
enum { MSG_QUEUE_LEN = 10 };  // Number of slots in message queue
String wspackage = " ";



#endif