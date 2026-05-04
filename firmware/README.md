# Firmware / 固件

---

## Overview / 项目简介

This firmware runs on an Arduino microcontroller and implements real-time sensing, communication, and force-feedback control for the haptic device.

The system reads joint motion from incremental encoders, measures gripper rotation using an AS5600 magnetic sensor, and communicates with an external system via serial. Based on incoming commands, it drives a DC motor to render haptic feedback.

本固件运行在 Arduino 微控制器上，实现触觉设备的实时数据采集、通信以及力反馈控制。

系统通过读取连杆上的增量式编码器获取关节运动信息，通过 AS5600 磁编码器测量夹爪角度，并通过串口与外部系统（如仿真环境）通信，根据接收到的控制指令驱动电机输出力反馈。

---

## Architecture / 系统架构

Although implemented in a single `.ino` file, the firmware is logically organized into several functional modules:

尽管代码目前以单个 `.ino` 文件实现，但在逻辑上可划分为以下模块：

* **Sensing（传感层）**

  * Read joint angles from incremental encoders
  * Read gripper angle from AS5600 (I2C)

* **State Processing（状态处理）**

  * Convert encoder counts to angles
  * Maintain system state (joint angles + gripper angle)

* **Communication（通信模块）**

  * Send real-time data via serial
  * Receive control parameters from host

* **Control（控制模块）**

  * Compute force feedback command
  * Drive DC motor using PWM

---

## Main Loop / 主循环结构

The firmware follows a structured loop:

固件采用结构化循环：

```cpp
void loop() {
    readEncoders();
    readGripper();
    computeState();
    sendData();
    receiveCommand();
    controlMotor();
}
```

This structure separates sensing, processing, communication, and actuation.

该结构实现了传感、处理、通信与控制的分层设计。

---

## Hardware Interface / 硬件接口

### Incremental Encoders (Model 3806) / 增量式编码器

* Quadrature signals (A/B channels)
* Used for joint angle measurement
* High-resolution relative motion tracking

用于关节角度测量，通过A/B相实现高分辨率位置跟踪。

---

### AS5600 Magnetic Encoder / 磁编码器

* I2C communication (SDA, SCL)
* Absolute angle measurement (0–360°)

用于夹爪角度测量，提供绝对角度信息。

---

### Motor Driver / 电机驱动

* PWM output for torque control
* Direction controlled via digital pins

通过PWM控制输出力矩，实现力反馈。

---

## Serial Communication / 串口通信

### Output (Arduino → Host) / 输出

```text
< x, y, alpha >
```

* `x, y`: end-effector position
* `alpha`: gripper angle

---

### Input (Host → Arduino) / 输入

* Contact threshold
* Stiffness parameter

用于计算力反馈控制信号。

---

## Control Logic / 控制逻辑

Force feedback is implemented using a simple virtual wall model:

系统采用虚拟墙模型实现力反馈：

```cpp
float error = contact_angle - alpha;
float torque = k * error;
```

The torque is converted into a PWM signal to drive the motor.

根据误差计算力矩，并转换为PWM信号驱动电机。

---

## Key Engineering Issues / 关键工程问题

### Encoder Drift / 编码器漂移

* Issue: system failed to return to zero
* Cause: loose encoder mounting
* Fix: rigid mounting using screws

---

### AS5600 Alignment / 传感器对齐

* Issue: unstable angle readings
* Cause: magnet misalignment
* Fix: adjust alignment and maintain 1–3 mm gap

---

### Real-Time Performance / 实时性

* Continuous loop ensures real-time response
* Serial communication optimized to avoid blocking

---

## File Location / 文件位置

```bash
arduino_main/arduino_main.ino
```

---

## Future Work / 后续优化

* Refactor into modular `.h/.cpp` structure
* Implement interrupt-based encoder decoding
* Improve control strategy (e.g., impedance control)
* Add filtering for noise reduction

后续可优化为模块化结构，并引入更高级控制与滤波方法。
