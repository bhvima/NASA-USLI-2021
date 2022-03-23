#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <ArduinoQueue.h>
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

ArduinoQueue<double> xValues(64);
ArduinoQueue<double> yValues(64);
double xTot = 0, yTot = 0;

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

  for(int i = 0; i < 64; i++) {
    tStart = micros();

    quat = bno.getQuat();
    accel = quat.rotateVector(accel);
    accel = accel - zero_ref;
    
    xValues.enqueue(accel.x());
    xTot = xTot + accel.x();
    yValues.enqueue(accel.y());
    yTot = yTot + accel.y();

    while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
  }

}

double xPos = 0, yPos = 0;
double xVel = 0, yVel = 0;
double xAcc = 0, yAcc = 0;

double dt = (double) (BNO055_SAMPLERATE_DELAY_MS) / 1000.0;

void loop(void) {

  unsigned long tStart = micros();

  xTot = xTot - xValues.dequeue();
  xValues.enqueue(accel.x());
  xTot = xTot + accel.x(); 

  yTot = yTot - yValues.dequeue();
  yValues.enqueue(accel.y());
  yTot = yTot + accel.y();

  xAcc = xTot/64;
  yAcc = yTot/64;

  xVel = xAcc * dt;
  xPos = xVel * dt;
  yVel = yAcc * dt;
  yPos = yVel * dt;

  Serial.print(xPos);
  Serial.print(" ");
  Serial.print(yPos);
  Serial.print(" ");
  Serial.print(xVel);
  Serial.print(" ");
  Serial.print(yVel);
  Serial.print(" ");
  Serial.print(xAcc);
  Serial.print(" ");
  Serial.println(yAcc);

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
}