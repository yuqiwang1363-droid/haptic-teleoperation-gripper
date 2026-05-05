# Experiments and Iterations

## Overview

This section documents the testing, debugging, and iterative design process of the system.

Instead of only presenting final results, this section highlights key engineering challenges and how they were resolved.

---

## Contents

* **Encoder Drift Issue**

  * Problem: system could not return to zero position
  * Cause: loose encoder mounting
  * Solution: rigid fixation

* **AS5600 Alignment**

  * Problem: unstable angle readings
  * Cause: magnet misalignment
  * Solution: adjusted alignment and spacing

* **Gripper Design Iteration**

  * Problem: poor tactile perception
  * Cause: excessive thickness
  * Solution: tapered geometry

* **User Interaction Improvement**

  * Problem: pinch interaction unstable
  * Solution: added vertical handle

---

## Purpose

These experiments demonstrate the importance of mechanical design, sensor integration, and iterative refinement in building a reliable haptic system.


## 实验与迭代过程

### 概述

本部分记录了系统在开发过程中的测试、调试以及结构迭代过程。

相比仅展示最终结果，这里重点说明了在实际工程实现中遇到的关键问题、分析过程以及对应的解决方案，从而体现系统从原型到稳定运行的优化过程。

---

### 内容概览

#### 编码器漂移问题（Encoder Drift）

* **问题**：连杆在运动后无法准确回到初始零位，存在明显误差
* **原因**：编码器与轴连接不牢，存在相对滑动
* **解决方案**：通过螺丝对编码器进行刚性固定
* **结果**：零位稳定性显著提升，位置误差明显降低

---

#### AS5600 对齐问题（Sensor Alignment）

* **问题**：夹爪角度测量不稳定，读数存在波动
* **原因**：磁铁与传感器未正确对齐，间距不合适
* **解决方案**：调整磁铁轴向位置并控制间距在 1–3 mm
* **结果**：角度测量稳定性明显改善

---

#### 夹爪结构优化（Gripper Design Iteration）

* **问题**：夹爪前端过厚，用户手指无法完全闭合，影响触觉感知
* **原因**：初始设计未考虑人手操作空间
* **解决方案**：对夹爪前端进行斜向减薄设计（tapered geometry）
* **结果**：显著提升手指接触效果和尺寸感知能力

---

#### 人机交互改进（User Interaction）

* **问题**：初始采用双指捏合方式操作，控制不稳定
* **原因**：缺乏稳定的操作支撑结构
* **解决方案**：增加垂直握把（vertical handle）
* **结果**：操作更加稳定，力反馈感知更自然

---

### 工程意义

上述实验与改进过程表明：

* 机械结构设计对系统精度具有决定性影响
* 传感器安装与对齐直接影响测量可靠性
* 人机交互设计对触觉系统体验至关重要

通过不断迭代优化，系统最终实现了稳定运行与良好的交互效果。

---

### 说明

各具体问题的详细分析与实现过程可见对应文档：

* `encoder_drift.md`
* `as5600_alignment.md`
* `gripper_design_iteration.md`
* `user_interaction.md`
