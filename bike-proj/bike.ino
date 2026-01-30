#include <Wire.h>
#include <Servo.h>

Servo servo180;
Servo servo270;

// --- TUNING ---
const float SENSITIVITY = 2.2;      
const float DEADZONE = 0.5;         
const float centeringSpeed = 0.4;   // Keeping your value
const int PIN_180 = 5; 
const int PIN_270 = 6;

// --- VARIABLES ---
float currentAngle = 135.0; // Master Center (Midpoint of 270)
float gyroZ_offset = 0;
unsigned long lastMicros;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  
  servo180.attach(PIN_180);
  servo270.attach(PIN_270);

  // [MPU Wakeup and Calibration logic goes here]
  // ... (Identical to your previous setup) ...

  servo180.write(90);   // Center for 180 servo
  servo270.write(135);  // Center for 270 servo
  
  lastMicros = micros();
}

void loop() {
  unsigned long currentMicros = micros();
  float dt = (currentMicros - lastMicros) / 1000000.0; 
  lastMicros = currentMicros;

  // Read Gyro...
  Wire.beginTransmission(0x68);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 2, true);
  float gyroZ = ((int16_t)(Wire.read() << 8 | Wire.read()) - gyroZ_offset) / 131.0;

  // 1. TRACK MOVEMENT
  if (abs(gyroZ) > DEADZONE) {
    currentAngle += (gyroZ * dt) * SENSITIVITY;
  } 
  // 2. AUTO-CENTER (Using your original logic style)
  else {
    if (currentAngle > 135.2) currentAngle -= centeringSpeed;
    else if (currentAngle < 134.8) currentAngle += centeringSpeed;
    else currentAngle = 135.0; 
  }

  // 3. LIMITS: Keep currentAngle within the 270° servo's wide capability
  // (Adjust 45 and 225 if you want even more or less sweep)
  currentAngle = constrain(currentAngle, 45, 225); 

  // 4. SERVO OUTPUTS
  // Servo 270: Follows the master angle directly
  int out270 = (int)currentAngle;
  
  // Servo 180: Scale the 270-range down to 180-range
  // This ensures 135 on the master becomes 90 on the 180-servo
  int out180 = map(out270, 45, 225, 45, 135);

  servo180.write(out180);
  servo270.write(out270);

  if (millis() % 100 == 0) {
    Serial.print("Master: "); Serial.print(currentAngle);
    Serial.print(" | S270: "); Serial.println(out270);
  }

  delay(10); 
}
