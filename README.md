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

This project was developed as part of a UCSD MAE course project.
