#include <Wire.h>
#include <Servo.h>

Servo servoLeft;
Servo servoRight;

// --- CONFIGURATION ---
const int PIN_LEFT = 5; 
const int PIN_RIGHT = 6;
float currentAngle = 90.0;  // Start position (Straight)
float gyroZ_offset = 0;     // To fix that -0.37 noise you saw

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  servoLeft.attach(PIN_LEFT);
  servoRight.attach(PIN_RIGHT);

  // Wake up MPU-9250
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // 1. AUTO-CALIBRATION
  // This calculates that "-0.37" offset automatically. 
  // DO NOT MOVE THE MPU DURING THESE 2 SECONDS.
  Serial.println("Calibrating... Keep MPU still!");
  long sum = 0;
  for (int i = 0; i < 500; i++) {
    Wire.beginTransmission(0x68);
    Wire.write(0x47); // Gyro Z
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 2, true);
    sum += (int16_t)(Wire.read() << 8 | Wire.read());
    delay(2);
  }
  gyroZ_offset = sum / 500.0;
  
  Serial.println("Calibration Done!");
  servoLeft.write(90);
  servoRight.write(135); // Initial center for 270 deg servo
}

void loop() {
  // 2. READ ROTATION SPEED
  Wire.beginTransmission(0x68);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 2, true);
  int16_t rawZ = (Wire.read() << 8 | Wire.read());

  // 3. REMOVE OFFSET & CONVERT TO DEG/SEC
  float gyroZ = (rawZ - gyroZ_offset) / 131.0;

  // 4. INTEGRATION (Speed * Time = Change in Angle)
  // We run at 20ms (0.02 seconds)
  if (abs(gyroZ) > 0.5) { // Deadzone to stop "creeping"
    currentAngle += (gyroZ * 0.02);
  }

  // 5. LIMITS (Keep fog lights from hitting the bike frame)
  currentAngle = constrain(currentAngle, 45, 135);

  // 6. UPDATE SERVOS
  int pos180 = (int)currentAngle;
  int pos270 = map(pos180, 45, 135, 90, 180);

  servoLeft.write(pos180);
  servoRight.write(pos270);

  // 7. MONITOR
  Serial.print("Rotation Speed: "); Serial.print(gyroZ);
  Serial.print(" | Light Angle: "); Serial.println(currentAngle);

  delay(20); 
}
