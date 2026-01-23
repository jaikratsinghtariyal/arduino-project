# 🏍️ Adaptive Motorcycle Fog Light System

This project implements an intelligent, self-leveling fog light system designed for a **Royal Enfield**. Using an **MPU-9250** IMU, the system calculates the motorcycle's rotation (Yaw) and adjusts dual servos to steer the light beams into corners, improving nighttime visibility.

## 🛠️ Hardware Specifications
- **Controller:** Arduino Nano
- **Sensor:** MPU-9250 (9-Axis Gyro/Accelerometer)
- **Actuators:** 2x 180° High-Torque Servos
- **Filtering:** Exponential Moving Average (EMA) to suppress engine vibration.

---

## 🔌 Connection Map

### 1. Sensor Connections (I2C)
| MPU-9250 Pin | Nano Pin | Description |
| :--- | :--- | :--- |
| **VCC** | 5V | Power (Red) |
| **GND** | GND | Ground (Black) |
| **SDA** | A4 | Data Line |
| **SCL** | A5 | Clock Line |

### 2. Servo Connections
| Component | Signal Pin | Power Source |
| :--- | :--- | :--- |
| **Left Servo** | **D9** | External 5V Buck Converter |
| **Right Servo** | **D10** | External 5V Buck Converter |

> ⚠️ **IMPORTANT:** You must share a **Common Ground (GND)** between the Arduino, the MPU-9250, and the external servo power supply to ensure signal stability.



---

## ⚙️ Software Parameters
The following constants at the top of the `.ino` file allow for fine-tuning based on your riding style:

| Variable | Default | Description |
| :--- | :--- | :--- |
| `CENTER_POS` | `90` | The "Straight Ahead" angle for the servos. |
| `MAX_ANGLE` | `45` | Maximum steering limit to protect the bike frame/forks. |
| `SENSITIVITY`| `1.2` | Multiplier for rotation (1.2 = lights turn 20% more than the bike). |
| `DRIFT_GATE` | `0.5` | Ignores rotation slower than 0.5°/sec (vibration rejection). |
| `SMOOTHING`  | `0.1` | Low-pass filter weight (0.1 = smooth/fluid, 1.0 = raw/jittery). |

---

## 📋 Installation & Calibration
1. **Mounting:** Secure the MPU-9250 flat and central to the motorcycle chassis.
2. **Startup:** Power the system while the bike is stationary (e.g., on the center stand).
3. **Auto-Calibration:** On boot, the system takes 200 samples to calculate the gyro offset and eliminate "phantom" rotation caused by sensor tilt.
4. **Operation:** The `smoothedAngle` logic ensures the lights track smoothly and ignore high-frequency engine "thumping."

---

## 🛠️ Future Roadmap
- [ ] Implement Magnetometer fusion to correct long-term Gyro drift.
- [ ] Add a physical "Recenter" toggle switch for on-the-fly adjustments.
