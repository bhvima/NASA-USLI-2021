#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &Wire1);

void displayAll()
{
  imu::Quaternion quat = bno.getQuat();
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel = quat.rotateVector(accel);
  Serial.print(accel.x());
  Serial.print(" ");
  Serial.print(accel.y());
  Serial.print(" ");
  Serial.print(accel.z());
  Serial.print(" ");
  
  Serial.print(quat.w(), 4);
  Serial.print(" ");
  Serial.print(quat.x(), 4);
  Serial.print(" ");
  Serial.print(quat.y(), 4);
  Serial.print(" ");
  Serial.print(quat.z(), 4);
  Serial.print(" ");

  uint8_t cg, ca;
  bno.getCalibration(NULL, &cg, &ca, NULL);
  Serial.print(cg, DEC);
  Serial.print(" ");
  Serial.println(ca, DEC);
}

void displayCalStatus()
{
  uint8_t gyro, accel;
  bno.getCalibration(NULL, &gyro, &accel, NULL);
  Serial.print("G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.println(accel, DEC);
}

void displayQuat()
{
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qZ: ");
  Serial.println(quat.z(), 4);
}

void displayLinAccel()
{
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  Serial.print("X: ");
  Serial.print(accel.x());
  Serial.print(" Y: ");
  Serial.print(accel.y());
  Serial.print(" Z: ");
  Serial.println(accel.z());
}

void setup(void) {
  Serial.begin(9600);

  /*  Initialize the sensor in Config mode */
  if(!bno.begin(Adafruit_BNO055::OPERATION_MODE_CONFIG)) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  

  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  bno.setExtCrystalUse(true);
}

void loop(void) {

  unsigned long tStart = micros();

  displayAll();

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
}