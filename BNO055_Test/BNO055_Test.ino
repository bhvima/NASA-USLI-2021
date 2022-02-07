#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

Adafruit_BNO055 bno = Adafruit_BNO055(-1, BNO055_ADDRESS_A, &Wire1);

void displayCalStatus(uint8_t gyro, uint8_t accel)
{
  Serial.print("G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.println(accel, DEC);
}

void setup(void) {
  Serial.begin(115200);

  /* 
  Initialize the sensor in IMU mode
  - In the IMU mode the relative orientation of the BNO055
    in space is calculated from the accelerometer and 
    gyroscope data.
  */
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

  /* Calibrate the sensors
     - Place the device in 6 different stable positions for a period
     of few seconds to allow the accelerometer to calibrate.
     - Make sure that there is slow movement between 2 stable positions
     - The 6 stable positions could be in any direction, but make sure
       that the device is lying at least once perpendicular to the x, y
       and z axis.
     - Place the device in a single stable position for a period of few
       seconds to allow the gyroscope to calibrate
  */
  uint8_t accel, gyro = 0;
  while(!(accel == 3 && gyro == 3)) {
    bno.getCalibration(NULL, &gyro, &accel, NULL);
    displayCalStatus(gyro, accel);
  }
  Serial.println("Sensor Calibrated");
}

void loop(void) {

  unsigned long tStart = micros();

  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qZ: ");
  Serial.println(quat.z(), 4);

  //Linear Acceleration
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  accel = quat.rotateVector(accel);
  Serial.print("X: ");
  Serial.print(accel.x());
  Serial.print(" Y: ");
  Serial.print(accel.y());
  Serial.print(" Z: ");
  Serial.println(accel.z());

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));
}
