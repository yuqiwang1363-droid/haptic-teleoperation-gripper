# Firmware

## Overview

This firmware is implemented on an Arduino microcontroller and is responsible for real-time sensing, data processing, and force-feedback control of the haptic device.

The system reads joint motion from incremental encoders, measures gripper rotation using an AS5600 magnetic sensor, communicates with an external system via serial, and drives a DC motor to render haptic feedback.

---

## Architecture

Although implemented in a single `.ino` file, the firmware is logically organized into modular functional blocks:

* **Sensing Layer**

  * Reads joint angles from incremental encoders (Model 3806)
  * Reads absolute angle from AS5600 sensor (I2C)

* **State Processing**

  * Converts raw encoder counts into angular values
  * Computes system state (joint angles and gripper angle)

* **Communication**

  * Sends real-time state data to host system via serial
  * Receives control parameters (e.g., contact threshold, stiffness)

* **Control**

  * Computes force-feedback command
  * Drives DC motor using PWM output

---

## Main Loop Structure

The firmware follows a structured control loop:

```cpp id="loop_struct"
void loop() {
    readEncoders();
    readGripper();
    computeState();
    sendData();
    receiveCommand();
    controlMotor();
}
```

This structure separates sensing, processing, communication, and actuation, improving readability and maintainability.

---

## Hardware Interface

### Incremental Encoders (Model 3806)

* Quadrature signals (A/B channels)
* Used for joint angle measurement
* High-resolution relative motion tracking

### AS5600 Magnetic Encoder

* I2C communication (SDA, SCL)
* Provides absolute angle measurement (0–360°)
* Used for gripper rotation sensing

### Motor Driver

* PWM output for torque control
* Direction controlled via digital pins

---

## Serial Communication

### Output (Arduino → Host)

Format:

```id="serial_out"
< x, y, alpha >
```

* `x, y` → end-effector position (or joint-derived values)
* `alpha` → gripper angle

---

### Input (Host → Arduino)

* Contact threshold
* Stiffness parameter

Used to compute force feedback.

---

## Control Logic

Force feedback is implemented using a simple virtual wall model:

```cpp id="control_model"
float error = contact_angle - alpha;
float torque = k * error;
```

The computed torque is converted to a PWM signal to drive the motor.

---

## Key Engineering Considerations

### Encoder Stability

* Loose mounting caused significant position drift
* Fixed using rigid mechanical attachment

### AS5600 Alignment

* Accurate sensing requires proper magnet alignment (1–3 mm gap)
* Misalignment leads to nonlinear readings

### Real-Time Performance

* Loop designed for continuous real-time execution
* Serial communication optimized to avoid blocking

---

## File Location

Main firmware file:

```bash id="fw_path"
arduino_main/arduino_main.ino
```

---

## Future Improvements

* Refactor into modular `.h/.cpp` structure
* Implement interrupt-based encoder decoding
* Improve control strategy (e.g., impedance control)
* Add filtering for sensor noise reduction


# 固件（Firmware）

## 项目简介

本固件运行在 Arduino 微控制器上，负责触觉设备的实时数据采集、状态计算以及力反馈控制。

系统通过读取连杆上的增量式编码器和夹爪上的 AS5600 磁编码器，获取用户输入的运动信息，并通过串口与上位机（或仿真环境）通信，最终驱动直流电机输出力反馈，实现硬件在环交互。

---

## 系统架构

虽然代码目前以单个 `.ino` 文件实现，但在逻辑上划分为以下模块：

* **传感层（Sensing Layer）**

  * 读取 3806 增量式编码器信号（关节角度）
  * 读取 AS5600 磁编码器（夹爪角度）

* **状态处理（State Processing）**

  * 将编码器计数转换为角度
  * 计算系统状态（关节角度与夹爪角度）

* **通信模块（Communication）**

  * 通过串口发送实时数据
  * 接收来自仿真环境的控制参数（如接触阈值、刚度）

* **控制模块（Control）**

  * 计算力反馈输出
  * 通过 PWM 驱动电机

---

## 主循环结构

固件采用结构化的循环流程：

```cpp id="loop_cn"
void loop() {
    readEncoders();
    readGripper();
    computeState();
    sendData();
    receiveCommand();
    controlMotor();
}
```

该结构实现了“传感 → 处理 → 通信 → 控制”的清晰分层，便于维护和扩展。

---

## 硬件接口

### 增量式编码器（3806）

* 正交信号（A/B 相）
* 用于关节角度测量
* 提供高分辨率相对位移信息

---

### AS5600 磁编码器

* 通过 I2C（SDA / SCL）通信
* 输出 0–360° 绝对角度
* 用于夹爪角度测量

---

### 电机驱动

* 使用 PWM 控制输出力矩
* 通过数字引脚控制转动方向

---

## 串口通信

### 输出（Arduino → 上位机）

数据格式：

```id="serial_cn"
< x, y, alpha >
```

* `x, y`：末端位置（或由关节计算得到）
* `alpha`：夹爪角度

---

### 输入（上位机 → Arduino）

* 接触阈值（contact threshold）
* 虚拟刚度参数（stiffness）

用于生成力反馈控制信号。

---

## 控制逻辑

系统采用简单的虚拟墙模型实现力反馈：

```cpp id="control_cn"
float error = contact_angle - alpha;
float torque = k * error;
```

根据误差计算输出力矩，并转换为 PWM 信号驱动电机。

---

## 关键工程问题

### 编码器漂移问题

* 问题：系统无法稳定回到零位
* 原因：编码器固定不牢
* 解决：使用螺丝固定，消除误差

---

### AS5600 对齐问题

* 问题：角度测量不稳定
* 原因：磁铁与传感器未对齐
* 解决：调整轴向位置并控制间距（1–3 mm）

---

### 实时性问题

* 采用连续循环结构保证实时执行
* 串口通信避免阻塞，提高响应速度

---

## 文件位置

主程序文件：

```bash id="fw_cn_path"
arduino_main/arduino_main.ino
```

---

## 后续优化方向

* 将代码拆分为 `.h/.cpp` 模块化结构
* 使用中断方式提升编码器读取精度
* 引入更高级控制策略（如阻抗控制）
* 增加滤波算法降低噪声影响
