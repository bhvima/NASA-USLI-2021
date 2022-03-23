#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &Wire1);

double xPos = 0, yPos = 0, zPos = 0;
double xVel = 0, yVel = 0, zVel = 0;
double xAcc = 0, yAcc = 0, zAcc = 0;

double cutoff = 0.05;

double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;

void displayAll()
{
  imu::Quaternion quat = bno.getQuat();
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel = quat.rotateVector(accel);

  xAcc = (1-cutoff) * xAcc + cutoff * accel.x();
  yAcc = (1-cutoff) * yAcc + cutoff * accel.y();
  zAcc = (1-cutoff) * zAcc + cutoff * accel.z();

  if(abs(xAcc) < 0.1) {
    xAcc = 0;
  }

  if(abs(yAcc) < 0.1) {
    yAcc = 0;
  }

  if(abs(zAcc) < 0.1) {
    zAcc = 0;
  }

  xPos = xPos + xVel * ACCEL_VEL_TRANSITION + xAcc * ACCEL_POS_TRANSITION;
  yPos = yPos + yVel * ACCEL_VEL_TRANSITION + yAcc * ACCEL_POS_TRANSITION;
  zPos = zPos + zVel * ACCEL_VEL_TRANSITION + zAcc * ACCEL_POS_TRANSITION;

  xVel = xVel + xAcc * ACCEL_VEL_TRANSITION;
  yVel = yVel + yAcc * ACCEL_VEL_TRANSITION;
  zVel = zVel + zAcc * ACCEL_VEL_TRANSITION;

  Serial.print(xVel);
  Serial.print(" ");
  Serial.print(yVel);
  Serial.print(" ");
  Serial.print(zVel);
  Serial.print(" ");
  Serial.println("");

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

  /*  Initialize the sensor in IMU mode */
  if(!bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS)) {
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

  while(!bno.isFullyCalibrated()) {
    displayCalStatus();
  }
  Serial.println("Sensor Calibrated");

}

void loop(void) {

  unsigned long tStart = micros();

  displayAll();

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
}