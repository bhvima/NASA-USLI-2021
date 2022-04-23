#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "Filter.h"

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)
#define CUTOFF (0.01)
#define ALPHA (0.99)

Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &Wire1);

void displayCalibrationStatus()
{
  uint8_t gyro, accel;
  bno.getCalibration(NULL, &gyro, &accel, NULL);
  Serial.print("G:");
  Serial.print(gyro);
  Serial.print(" A:");
  Serial.println(accel);
}

unsigned long tStart;

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
    displayCalibrationStatus();
  }

  Serial.println("Sensor Calibrated");
}

imu::Quaternion quat;
imu::Vector<3> accel;

double prev_acc_x = 0, prev_acc_y = 0;
double acc_x = 0, acc_y = 0;

double prev_vel_x = 0, prev_vel_y = 0;
double vel_x = 0, vel_y = 0;

double prev_x = 0, prev_y = 0;
double pos_x = 0, pos_y = 0;


double dt = 0.01;

void loop(void) {

  tStart = millis();

  quat = bno.getQuat();
  accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel = quat.rotateVector(accel);

  /* Low pass filter */
  double raw_acc_x = accel.x();
  double raw_acc_y = accel.y();

  acc_x = (1 - CUTOFF) * acc_x + 0.5 * CUTOFF * (prev_acc_x + raw_acc_x);

  if (abs(acc_x) < 0.01) {
    acc_x = 0;
  }

  acc_y = (1 - CUTOFF) * acc_y + 0.5 * CUTOFF * (prev_acc_y + raw_acc_y);

  if (abs(acc_y) < 0.01) {
    acc_y = 0;
  }

  prev_acc_x = raw_acc_x;
  prev_acc_y = raw_acc_y;

  double raw_vel_x = prev_vel_x + dt * acc_x;
  double raw_vel_y = prev_vel_y + dt * acc_y;

  vel_x = ALPHA * (vel_x + raw_vel_x - prev_vel_x);
  vel_y = ALPHA * (vel_y + raw_vel_y - prev_vel_y);

  prev_vel_x = raw_vel_x;
  prev_vel_y = raw_vel_y;

  double raw_x = prev_x + dt * vel_x;
  double raw_y = prev_y + dt * vel_y;

  pos_x = 0.999666777741 * (pos_x + raw_x - prev_x);
  pos_y = 0.999666777741 * (pos_y + raw_y - prev_y);

  Serial.print(acc_x);
  Serial.print(" ");
  Serial.print(acc_y);
  Serial.print(" ");
  Serial.print(vel_x);
  Serial.print(" ");
  Serial.print(vel_y);
  Serial.print(" ");
  Serial.print(pos_x);
  Serial.print(" ");
  Serial.println(pos_y);

  while ((millis() - tStart) < BNO055_SAMPLERATE_DELAY_MS);
}