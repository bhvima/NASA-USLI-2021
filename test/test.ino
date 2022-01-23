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
}

double xPos = 0, yPos = 0, headingVel = 0;
double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251;

void loop(void) {

  unsigned long tStart = micros();

  sensors_event_t orientationData , linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
  yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;

  // velocity of sensor in the direction it's facing
  headingVel = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.x / cos(DEG_2_RAD * orientationData.orientation.x);

  Serial.print("Heading: ");
  Serial.println(orientationData.orientation.x);
  Serial.print("Position: ");
  Serial.print(xPos);
  Serial.print(" , ");
  Serial.println(yPos);
  Serial.print("Speed: ");
  Serial.println(headingVel);
  Serial.println("-------");

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000))
  {
    //poll until the next sample is ready
  }

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