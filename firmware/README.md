# Firmware

## Overview

This firmware runs on an Arduino microcontroller and is responsible for real-time sensing, data processing, and force-feedback control.

It reads encoder signals, computes joint angles, communicates with an external system, and drives the motor to render haptic feedback.

---

## Features

* Real-time quadrature decoding for incremental encoders
* Absolute angle measurement using AS5600 (I2C)
* Forward kinematics computation (optional)
* Serial communication with host system
* Motor control for haptic force rendering

---

## Hardware Interface

### Encoders (Model 3806)

* Digital input pins (A/B channels)
* Interrupt-based or polling-based decoding

### AS5600

* I2C communication (SDA, SCL)
* Provides absolute angle (0–360°)

### Motor Driver

* PWM output for motor control
* Direction control via digital pins

---

## Main Functional Flow

1. Read encoder signals (joint angles)
2. Read AS5600 angle (gripper)
3. Compute system state
4. Send data to host (serial)
5. Receive force command from host
6. Drive motor accordingly

---

## Serial Communication

### Output (Arduino → Host)

Format:

```
< x, y, alpha >
```

### Input (Host → Arduino)

* Contact angle
* Stiffness parameter

---

## Key Functions

* `readEncoders()` → get joint angles
* `readAS5600()` → get gripper angle
* `computeState()` → process data
* `sendData()` → serial output
* `readCommand()` → receive feedback parameters
* `controlMotor()` → generate force feedback

---

## Notes

* Proper encoder mounting is critical for accurate readings
* AS5600 requires correct magnet alignment (1–3 mm gap)
* Serial baud rate typically set to 115200

---

## Location

Main firmware:

```bash
arduino_main/arduino_main.ino
```

