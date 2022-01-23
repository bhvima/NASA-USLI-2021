#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

Adafruit_BNO055 bno = Adafruit_BNO055();

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
  Serial.println("");

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
  }
  Serial.println("Sensor Calibrated");
}

void loop(void) {

  unsigned long tStart = micros();

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000));

  /*
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  Serial.print("X: ");
  Serial.print(euler.x());
  Serial.print(" Y: ");
  Serial.print(euler.y());
  Serial.print(" Z: ");
  Serial.print(euler.z());
  Serial.print("\n");

  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  Serial.print("X: ");
  Serial.print(accel.x());
  Serial.print(" Y: ");
  Serial.print(accel.y());
  Serial.print(" Z: ");
  Serial.print(accel.z());
  Serial.print("\n");
  */
}