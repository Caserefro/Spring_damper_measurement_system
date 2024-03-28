#ifndef functions.h
#define functions_h
#include "definitions.h"



/*
void IRAM_ATTR SECOND_IMU_ISR_FLAG() {
  SECOND_IMUReadingAvailable = true;
}
void IRAM_ATTR THIRD_IMU_ISR_FLAG() {
  THIRD_IMUReadingAvailable = true;
}*/

uint8_t readMPU9250Registerfunc(SPIClass* spi, uint8_t csPin, uint8_t reg) {
  uint8_t regValue = 0;

  reg |= 0x80;
  spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  spi->transfer(reg);
  regValue = spi->transfer(0x00);
  digitalWrite(csPin, HIGH);
  spi->endTransaction();

  return regValue;
}

void writeMPU9250Registerfunc(SPIClass* spi, uint8_t csPin, uint8_t reg, uint8_t val) {
  spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(csPin, LOW);
  spi->transfer(reg);
  spi->transfer(val);
  digitalWrite(csPin, HIGH);
  spi->endTransaction();
}

void enableINTmpu6500(uint8_t csPin) {
  uint8_t regVal;
  regVal = readMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_PIN_CFG);  //POLARITY
  regVal &= ~(0x80);
  writeMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_PIN_CFG, regVal);

  regVal = readMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_PIN_CFG);  //LATCH
  regVal &= ~(0x20);
  writeMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_PIN_CFG, regVal);

  regVal = readMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_PIN_CFG);  //CLEAR INT BY ANY
  regVal |= 0x10;
  writeMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_PIN_CFG, regVal);

  regVal = readMPU9250Registerfunc(&SPI, csPin, 0x40);  //ENABLE INT
  regVal |= 0x40;
  writeMPU9250Registerfunc(&SPI, csPin, REGISTER_INT_ENABLE, 0x49);
}

// had problems with the SPI PROTOCOL ------(SKILL ISSUE IN MY PART, FIX IN FUTURE OPTIMIZATIONS.)
void init_FIRST_MPU6500() {
  if (!FIRST_MPU6500.init()) {
    Serial.println("FIRST MPU6500 does not respond");
  } else {
    Serial.println("FIRST MPU6500 is connected");
  }

  pinMode(FIRST_IMU_ISR_PIN, INPUT);
  /* Choose the SPI clock speed, default is 8 MHz 
     This function must be used only after init(), not before */
  //myMPU9250.setSPIClockSpeed(4000000);

  Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
  delay(1000);
  FIRST_MPU6500.autoOffsets();
  Serial.println("Done!");

  //FIRST_MPU6500.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);
  //FIRST_MPU6500.setGyrOffsets(45.0, 145.0, -105.0);
  FIRST_MPU6500.enableGyrDLPF();
  //FIRST_MPU6500.disableGyrDLPF(MPU6500_BW_WO_DLPF_8800); // bandwdith without DLPF
  FIRST_MPU6500.setGyrDLPF(MPU6500_DLPF_6);
  FIRST_MPU6500.setSampleRateDivider(5);
  FIRST_MPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  FIRST_MPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
  FIRST_MPU6500.enableAccDLPF(true);
  FIRST_MPU6500.setAccDLPF(MPU6500_DLPF_6);
  //FIRST_MPU6500.enableAccAxes(MPU6500_ENABLE_XYZ);
  //FIRST_MPU6500.enableGyrAxes(MPU6500_ENABLE_XYZ);
}
/*
void init_SECOND_MPU6500() {
  if (!SECOND_MPU6500.init()) {
    Serial.println("SECOND MPU6500 does not respond");
  } else {
    Serial.println("SECOND MPU6500 is connected");
  }

  pinMode(SECOND_IMU_ISR_PIN, INPUT);
  attachInterrupt(SECOND_IMU_ISR_PIN, SECOND_IMU_ISR_FLAG, RISING);
  /* Choose the SPI clock speed, default is 8 MHz 
     This function must be used only after init(), not before 
  //myMPU9250.setSPIClockSpeed(4000000);

  Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
  delay(1000);
  SECOND_MPU6500.autoOffsets();
  Serial.println("Done!");

  //SECOND_MPU6500.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);
  //SECOND_MPU6500.setGyrOffsets(45.0, 145.0, -105.0);
  SECOND_MPU6500.enableGyrDLPF();
  //SECOND_MPU6500.disableGyrDLPF(MPU6500_BW_WO_DLPF_8800); // bandwdith without DLPF
  SECOND_MPU6500.setGyrDLPF(MPU6500_DLPF_6);
  SECOND_MPU6500.setSampleRateDivider(5);
  SECOND_MPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  SECOND_MPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
  SECOND_MPU6500.enableAccDLPF(true);
  SECOND_MPU6500.setAccDLPF(MPU6500_DLPF_6);
  //SECOND_MPU6500.enableAccAxes(MPU6500_ENABLE_XYZ);
  //SECOND_MPU6500.enableGyrAxes(MPU6500_ENABLE_XYZ);
}

/*
void init_THIRD_MPU6500() {
  if (!THIRD_MPU6500.init()) {
    Serial.println("THIRD MPU6500 does not respond");
  } else {
    Serial.println("THIRD MPU6500 is connected");
  }

  pinMode(THIRD_IMU_ISR_PIN, INPUT);
  attachInterrupt(THIRD_IMU_ISR_PIN, THIRD_IMU_ISR_FLAG, RISING);
  /* Choose the SPI clock speed, default is 8 MHz 
     This function must be used only after init(), not before 
  //myMPU9250.setSPIClockSpeed(4000000);

  Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
  delay(1000);
  THIRD_MPU6500.autoOffsets();
  Serial.println("Done!");

  //THIRD_MPU6500.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);
  //THIRD_MPU6500.setGyrOffsets(45.0, 145.0, -105.0);
  THIRD_MPU6500.enableGyrDLPF();
  //THIRD_MPU6500.disableGyrDLPF(MPU6500_BW_WO_DLPF_8800); // bandwdith without DLPF
  THIRD_MPU6500.setGyrDLPF(MPU6500_DLPF_6);
  THIRD_MPU6500.setSampleRateDivider(5);
  THIRD_MPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  THIRD_MPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
  THIRD_MPU6500.enableAccDLPF(true);
  THIRD_MPU6500.setAccDLPF(MPU6500_DLPF_6);
  //THIRD_MPU6500.enableAccAxes(MPU6500_ENABLE_XYZ);
  //THIRD_MPU6500.enableGyrAxes(MPU6500_ENABLE_XYZ);
}*/

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {

  if (type == WS_EVT_CONNECT) {

    Serial.println("Websocket client connection received");
    client->text("Hello from ESP32 Server");

  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Client disconnected");
  }
}



#endif