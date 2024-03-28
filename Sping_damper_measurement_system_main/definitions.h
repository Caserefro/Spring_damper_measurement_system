#ifndef definitions.h
#define definitions_h
#include "Credentials.h"

// MPU THINGS --------------------------------------------------------------------------------
#include <MPU6500_WE.h>

//FIRST MPU 6500
const int csPinMPU1 = 15;  // Chip Select Pin
const int FIRST_IMU_ISR_PIN = 2;
bool FIRST_IMUReadingAvailable = 0;

//SECOND MPU 6500
const int csPinMPU2 = 16;  // Chip Select Pin
const int SECOND_IMU_ISR_PIN = 4;
bool SECOND_IMUReadingAvailable = 0;

//THIRD MPU 6500
const int csPinMPU3 = 17;  // Chip Select Pin
const int THIRD_IMU_ISR_PIN = 5;
bool THIRD_IMUReadingAvailable = 0;

// Registers and configs used (Regs can be found in the MPU6500_WE library)
static uint8_t constexpr REGISTER_INT_PIN_CFG = 0x37;
static uint8_t constexpr REGISTER_INT_ENABLE = 0x38;
bool useSPI = true;  // SPI use flag

/* Constructors */
MPU6500_WE FIRST_MPU6500 = MPU6500_WE(&SPI, csPinMPU1, useSPI);
MPU6500_WE SECOND_MPU6500 = MPU6500_WE(&SPI, csPinMPU2, useSPI);
MPU6500_WE THIRD_MPU6500 = MPU6500_WE(&SPI, csPinMPU3, useSPI);

int aux;
int measures = 100;
int measures2 = 100;
int measures3 = 100;



#endif