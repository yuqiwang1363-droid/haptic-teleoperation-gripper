# AS5600 Sensor Alignment Issue

---

## Overview

During testing, the gripper angle measurement using the AS5600 magnetic encoder was found to be unstable and inconsistent.

The readings exhibited noise and nonlinearity, which affected the accuracy of force feedback control.

---

## Problem Description

* The measured angle fluctuated even when the handle was stationary
* The output was not smooth during rotation
* Angle readings were inconsistent across repeated movements

This behavior indicated an issue with the sensing setup rather than the control logic.

---

## Root Cause Analysis

The issue was caused by improper alignment between the magnet and the AS5600 sensor:

* The magnet was not centered above the sensor
* The axial distance between the magnet and sensor was not optimal
* The magnetic field distribution was uneven

These factors resulted in inaccurate angle detection.

---

## Solution

To improve measurement accuracy, the following adjustments were made:

* Carefully aligned the magnet with the center of the AS5600 sensor
* Adjusted the axial distance to approximately 1–3 mm
* Ensured the magnet remained stable during rotation

---

## Results

After alignment:

* Angle readings became stable and smooth
* Measurement noise was significantly reduced
* Sensor output became consistent across repeated tests

---

## Engineering Insight

This issue highlights that:

> Magnetic sensors are highly sensitive to alignment and positioning.

Proper mechanical alignment is essential to ensure accurate and reliable measurements.

---

---

# AS5600 传感器对齐问题

---

## 概述

在系统调试过程中发现，使用 AS5600 磁编码器测量夹爪角度时，读数不稳定且存在明显波动。

该问题影响了力反馈控制的准确性。

---

## 问题描述

* 手柄静止时角度读数仍然波动
* 旋转过程中输出不平滑
* 多次测试结果不一致

该现象表明问题出现在传感器安装，而非控制算法。

---

## 原因分析

问题主要来源于磁铁与 AS5600 传感器之间的对齐不良：

* 磁铁未对准传感器中心
* 轴向距离不合适
* 磁场分布不均匀

导致角度测量误差和不稳定现象。

---

## 解决方案

针对该问题进行了如下调整：

* 将磁铁精确对准传感器中心位置
* 控制磁铁与传感器间距在 1–3 mm 范围内
* 确保磁铁在旋转过程中保持稳定

---

## 改进效果

调整后：

* 角度读数稳定且连续
* 噪声明显降低
* 多次测试结果一致性显著提升

---

## 工程启示

该问题说明：

> 磁编码器对安装精度高度敏感。

只有在正确对齐和合理间距下，才能获得稳定可靠的测量结果。
