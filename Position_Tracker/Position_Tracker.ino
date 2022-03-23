#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

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

imu::Quaternion quat;
imu::Vector<3> accel;
imu::Vector<3> zero_ref;

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
  
  unsigned long tStart = micros();

  double ref_ax, ref_ay, ref_az;

  for(int i = 0; i < 0x400; i++) {
    tStart = micros();

    accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    ref_ax = ref_ax + accel.x();
    ref_ay = ref_ay + accel.y();
    ref_az = ref_az + accel.z();

    while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
  }

  zero_ref = imu::Vector<3>(ref_ax, ref_ay, ref_az);
  zero_ref = zero_ref/1024;

  Serial.println("Zero Reference Calculated");
}

imu::Vector<3> avg_accel;

double xPos = 0, yPos = 0;
double xVel = 0, yVel = 0;

double dt = (double) (64 * BNO055_SAMPLERATE_DELAY_MS) / 1000.0;

int i = 0;

void loop(void) {

  unsigned long tStart = micros();

  quat = bno.getQuat();
  accel = quat.rotateVector(accel);
  accel = accel - zero_ref;

  avg_accel = avg_accel + accel;

  if(i == 63) {
    avg_accel = avg_accel/64;
    xVel = avg_accel.x() * dt;
    xPos = xVel * dt;
    yVel = avg_accel.y() * dt;
    yPos = yVel * dt;

    Serial.print(xPos);
    Serial.print(" ");
    Serial.println(yPos);
  }

  i = (i + 1) % 64;

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
}