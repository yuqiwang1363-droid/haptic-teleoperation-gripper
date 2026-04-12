# Haptic Teleoperation Gripper System

## Overview

This project presents a hardware-in-the-loop (HIL) haptic device designed for teleoperation and human-machine interaction. The system consists of a 2-DOF planar linkage for position input and a 1-DOF force-feedback gripper that enables users to perceive virtual object interactions through kinesthetic feedback.

The device captures user motion using encoders and renders force feedback via a DC motor, forming a closed-loop interaction with a virtual environment.

---

## System Architecture

* **Mechanical System**

  * 2-DOF planar linkage for end-effector positioning
  * 1-DOF gripper handle with integrated DC motor for force feedback

* **Sensing**

  * Incremental encoders (Model 3806) for joint angle measurement
  * AS5600 magnetic encoder for absolute gripper angle sensing

* **Embedded System**

  * Arduino-based real-time processing
  * Quadrature decoding and angle computation
  * Motor control for haptic rendering

* **Interface**

  * Serial communication with external simulation (e.g., PyBullet)
  * Enables hardware-in-the-loop interaction

---

## My Contributions

* Designed the **2-DOF linkage mechanism** and **force-feedback handle**
* Performed **component selection** (encoders, motor, sensors)
* Integrated **incremental encoders and AS5600 sensor**
* Developed **Arduino firmware** for sensing and motor control
* Implemented **real-time data acquisition and actuation pipeline**
* Diagnosed and resolved key issues:

  * Encoder drift due to improper mounting
  * Sensor-magnet misalignment
  * Mechanical friction and usability issues
* Optimized gripper geometry and ergonomics for improved user interaction

---

## Hardware

* 2-DOF planar linkage (custom CAD design)
* RS380 DC motor for force feedback
* AS5600 magnetic encoder (I2C)
* Model 3806 incremental encoders (quadrature)
* Bearing-supported rotating handle

See `/hardware` for CAD files, wiring diagrams, and BOM.

---

## Firmware

The Arduino firmware performs:

* Encoder signal decoding (quadrature)
* Angle computation and calibration
* Serial communication
* Motor control for force feedback

Located in:

```bash
/firmware/arduino_main/
```

---

## Control and Modeling

* Forward kinematics for 2R planar linkage
* Real-time position tracking
* Haptic rendering using a virtual wall model
* Force output based on stiffness and penetration depth

See `/control` for details.

---

## Hardware-Software Interface

The system communicates with an external simulation via serial:

**Output (Arduino → Simulation):**

* End-effector position (x, y)
* Gripper angle (α)

**Input (Simulation → Arduino):**

* Contact threshold
* Virtual stiffness

This enables real-time hardware-in-the-loop interaction.

---

## Results

* Stable position tracking after mechanical refinement
* Improved accuracy through proper encoder mounting
* Smooth and responsive force feedback
* Enhanced user interaction through ergonomic design improvements

---

## Project Structure

```bash
hardware/      # CAD, drawings, electronics
firmware/      # Arduino code
control/       # kinematics and haptic models
experiments/   # testing and observations
docs/          # report and figures
interface/     # communication protocol
```

---

## Demo

(Add GIF or video here)

---

## Future Work

* Improve sensing accuracy and calibration
* Reduce mechanical friction and backlash
* Enhance force feedback with advanced control strategies
* Integrate more complex virtual environments

---

## Acknowledgment
This project was conducted at the University of California, San Diego under the supervision of Professor Tania Morimoto.



## 中文说明（Chinese Version）

### 项目简介

本项目设计并实现了一种基于硬件在环（Hardware-in-the-Loop, HIL）的触觉遥操作装置。系统由一个二维平面连杆机构（2-DOF）和一个具有力反馈功能的夹爪手柄（1-DOF）组成，实现了用户动作输入与虚拟环境交互之间的闭环控制。

系统通过传感器采集用户运动信息，并利用直流电机输出力反馈，使用户能够感知虚拟物体的接触与刚度特性，从而提升人机交互的真实感。

---

### 系统组成

* **机械结构**

  * 二自由度平面连杆机构（位置输入）
  * 一自由度力反馈手柄（夹爪）

* **传感器系统**

  * 3806 增量式编码器（关节角度测量）
  * AS5600 磁编码器（夹爪绝对角度）

* **嵌入式系统**

  * 基于 Arduino 的实时数据采集与控制
  * 编码器信号解码与角度计算
  * 电机驱动与力反馈控制

* **系统接口**

  * 通过串口与外部仿真环境通信
  * 支持硬件在环实时交互

---

### 本人工作内容

* 完成 **连杆机构与力反馈手柄的机械设计**
* 进行 **关键元件选型**（编码器、电机、传感器等）
* 实现 **传感器集成与系统搭建**
* 开发 **Arduino嵌入式程序**，实现数据采集与力反馈控制
* 搭建 **完整硬件系统并完成调试**
* 解决关键工程问题：

  * 编码器安装不牢导致的测量误差
  * AS5600 磁铁对齐问题
  * 机械摩擦与结构稳定性问题
* 优化夹爪结构与人机交互设计，提高操作体验

---

### 控制与建模

* 基于平面2R机构的正运动学进行末端位置计算
* 实现实时位置跟踪与数据映射
* 基于虚拟墙模型实现力反馈（刚度调制）
* 实现接触检测与力输出控制

---

### 系统通信

系统通过串口实现与外部仿真环境的数据交互：

* 输出（Arduino → 仿真）

  * 末端位置 (x, y)
  * 夹爪角度 (α)

* 输入（仿真 → Arduino）

  * 接触阈值
  * 虚拟刚度参数

实现硬件与虚拟环境的闭环交互。

---

### 实验结果

* 通过机械优化实现稳定的位置跟踪
* 编码器固定后显著降低零点漂移
* 力反馈响应平滑且稳定
* 结构优化后显著提升用户交互体验

---

### 后续工作

* 提升传感器精度与系统标定方法
* 降低机械摩擦与间隙误差
* 优化力反馈控制算法
* 扩展更复杂的虚拟交互环境

---

### 致谢

本项目在加州大学圣地亚哥分校完成，感谢 Tania Morimoto 教授的指导与支持。
