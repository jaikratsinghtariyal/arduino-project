#include <Wire.h>

long gyroZ, gyroZ_cal;
float angleZ = 0;
float displayAngle = 90; // Starting at your preferred 90 degrees
unsigned long lastTime;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Wake up MPU
  Wire.beginTransmission(0x68);
  Wire.write(0x6b);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("Calibrating... Keep sensor still.");
  // Calibration loop: Find the "Zero" error of your specific sensor
  for (int i = 0; i < 200; i++) {
    Wire.beginTransmission(0x68);
    Wire.write(0x47); // Gyro Z registers
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 2, true);
    gyroZ_cal += Wire.read() << 8 | Wire.read();
    delay(5);
  }
  gyroZ_cal /= 200;
  
  Serial.println("Done! Straight ahead is now 90.00");
  lastTime = micros(); // Using microseconds for better precision
}

void loop() {
  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;
  lastTime = currentTime;

  Wire.beginTransmission(0x68);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 2, true);
  gyroZ = Wire.read() << 8 | Wire.read();

  // Subtract the calibration error
  float actualRotZ = (gyroZ - gyroZ_cal) / 131.0;

  // Drift Gate: Ignore tiny movements so the angle doesn't crawl
  if (abs(actualRotZ) > 0.3) {
    angleZ += actualRotZ * dt;
  }

  // Apply the 90 degree base offset
  displayAngle = 90 + angleZ;

  // Constrain to Servo limits (0 to 180)
  displayAngle = constrain(displayAngle, 0, 180);

  Serial.print("Sensor Raw: "); Serial.print(angleZ);
  Serial.print(" | FOG LIGHT ANGLE: "); Serial.println(displayAngle);

  delay(1000); 
}
