#include <Wire.h>
#include <Servo.h>

// Servo Objects
Servo servoLeft;  
Servo servoRight; 

long gyroZ, gyroZ_cal;
float angleZ = 0;
float smoothedAngle = 90; 
unsigned long lastTime;

// --- TUNING CONSTANTS (Modify these for your bike) ---
const int CENTER_POS = 90;
const int MAX_ANGLE = 45;
const float SENSITIVITY = 1.2;
const float DRIFT_GATE = 0.5;   // <--- RESTORED: Ignores vibrations
const float SMOOTHING = 0.1;    // <--- 0.1 is smooth, 1.0 is raw

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Assign Pins D9 and D10 to the Servo Objects
  servoLeft.attach(9);   
  servoRight.attach(10); 

  // Wake up MPU-9250
  Wire.beginTransmission(0x68);
  Wire.write(0x6b);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("Calibrating Sensor...");
  for (int i = 0; i < 200; i++) {
    Wire.beginTransmission(0x68);
    Wire.write(0x47); 
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 2, true);
    gyroZ_cal += Wire.read() << 8 | Wire.read();
    delay(5);
  }
  gyroZ_cal /= 200;
  
  // Center the "Virtual" servos immediately
  servoLeft.write(CENTER_POS);
  servoRight.write(CENTER_POS);
  
  lastTime = micros();
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

  float actualRotZ = (gyroZ - gyroZ_cal) / 131.0;

  // The logic that keeps your lights steady at a red light
  if (abs(actualRotZ) > DRIFT_GATE) {
    angleZ += actualRotZ * dt;
  }

  // Calculate target with sensitivity and apply smoothing
  float targetAngle = CENTER_POS + (angleZ * SENSITIVITY);
  smoothedAngle = (smoothedAngle * (1.0 - SMOOTHING)) + (targetAngle * SMOOTHING);

  // Safety constraint
  int finalServoPos = constrain((int)smoothedAngle, CENTER_POS - MAX_ANGLE, CENTER_POS + MAX_ANGLE);

  // COMMAND: Sends data to D9 and D10
  servoLeft.write(finalServoPos); 
  servoRight.write(finalServoPos); 

  // DEBUG OUTPUT
  Serial.print("Z-Angle: "); Serial.print(angleZ);
  Serial.print(" | Servo Output (D9/D10): "); Serial.println(finalServoPos);

  delay(20); 
}
