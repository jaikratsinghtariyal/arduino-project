#include <Wire.h>
#include <Servo.h>

Servo servo180;
Servo servo270;

// --- TUNING ---
const float SENSITIVITY = 2.5;  
const float DEADZONE = 1.0;     
const int PIN_180 = 5; 
const int PIN_270 = 6;

// --- VARIABLES ---
float currentAngle = 90.0; // Our master "straight ahead" reference
float gyroZ_offset = 0;
unsigned long lastMicros;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  
  servo180.attach(PIN_180);
  servo270.attach(PIN_270);

  // Wake up MPU
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Calibration (Keep MPU perfectly still!)
  long sum = 0;
  for (int i = 0; i < 500; i++) {
    Wire.beginTransmission(0x68);
    Wire.write(0x47);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 2, true);
    sum += (int16_t)(Wire.read() << 8 | Wire.read());
    delay(2);
  }
  gyroZ_offset = sum / 500.0;
  
  // Force both to center immediately
  servo180.write(90);
  servo270.write(135); // 270 deg servo center is 135
  
  lastMicros = micros();
}

void loop() {
  unsigned long currentMicros = micros();
  float dt = (currentMicros - lastMicros) / 1000000.0; 
  lastMicros = currentMicros;

  Wire.beginTransmission(0x68);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 2, true);
  int16_t rawZ = (Wire.read() << 8 | Wire.read());

  float gyroZ = (rawZ - gyroZ_offset) / 131.0;

  // MATH: Add movement to currentAngle
  if (abs(gyroZ) > DEADZONE) {
    currentAngle += (gyroZ * dt) * SENSITIVITY;
  }

  // LIMITS: Keep within a 90-degree total swing (45 left to 135 right)
  currentAngle = constrain(currentAngle, 45, 135);

  // --- SERVO COMMANDS ---
  // Servo 180: Direct mapping
  int out180 = (int)currentAngle;
  
  // Servo 270: Needs to be shifted because its 90 is not the center
  // If 180-servo is at 90 (center), 270-servo should be at 135
  int out270 = out180 + 45; 

  servo180.write(out180);
  servo270.write(out270);

  // DEBUGGING: Watch these numbers in Serial Monitor
  Serial.print("GyroZ: "); Serial.print(gyroZ);
  Serial.print(" | Angle: "); Serial.print(out180);
  Serial.print(" | S270: "); Serial.println(out270);
}
