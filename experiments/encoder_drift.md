# Encoder Drift Issue

---

## Overview

During testing, the system exhibited significant position errors: after moving the linkage and returning it to the initial configuration, the measured position did not return to zero.

This issue directly affected the accuracy of position tracking and system reliability.

---

## Problem Description

* The linkage could not consistently return to its initial (zero) position
* The measured joint angles showed noticeable drift
* The error accumulated over repeated movements

This behavior indicated that the sensing system was not reliably capturing the actual joint motion.

---

## Root Cause Analysis

The issue was traced to the mechanical mounting of the incremental encoders.

* The encoder shaft was not rigidly fixed to the rotating joint
* Small relative motion occurred between the encoder and the shaft
* This introduced cumulative measurement error over time

As a result, the encoder readings no longer reflected the true joint angle.

---

## Solution

To eliminate the relative motion, the encoder mounting was improved:

* The encoder was rigidly fixed using screws
* The connection between the encoder and the rotating shaft was tightened
* Mechanical stability of the mounting structure was increased

---

## Results

After applying the fix:

* The linkage reliably returned to the zero position
* Measurement drift was eliminated
* Position tracking accuracy was significantly improved

---

## Engineering Insight

This issue highlights an important principle:

> Accurate sensing depends not only on sensor resolution, but also on mechanical integration quality.

Even high-resolution encoders can produce large errors if they are not properly mounted.

---

---

# 编码器漂移问题

---

## 概述

在系统调试过程中发现：当连杆运动后再返回初始位置时，测量结果无法回到零位，存在明显误差。

该问题直接影响了系统的位置跟踪精度与稳定性。

---

## 问题描述

* 连杆无法稳定回到初始零位
* 关节角度测量存在明显漂移
* 多次运动后误差逐渐累积

该现象表明传感系统未能准确反映真实的机械运动。

---

## 原因分析

问题来源于增量式编码器的机械安装方式：

* 编码器与旋转轴之间未实现刚性固定
* 存在微小相对滑动
* 随着运动累积产生测量误差

因此，编码器读数与实际角度逐渐偏离。

---

## 解决方案

针对该问题，进行了如下改进：

* 使用螺丝对编码器进行刚性固定
* 加强编码器与旋转轴之间的连接
* 提高整体安装结构的稳定性

---

## 改进效果

改进后：

* 连杆可以稳定回到零位
* 漂移问题消失
* 位置测量精度显著提升

---

## 工程启示

该问题说明：

> 传感精度不仅取决于传感器本身，还高度依赖机械安装质量。

即使是高精度编码器，如果安装不可靠，也会导致严重误差。
