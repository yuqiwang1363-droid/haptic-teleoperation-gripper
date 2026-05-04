#include <math.h>
#include <Wire.h>

#define AS5600_I2C_ADDRESS 0x36

// ---------------- Force-feedback motor driver ----------------
// Use a Hapkit-style interface: one PWM pin + one direction pin.
// Update these pins if your driver wiring is different.
const int MOTOR_PWM_PIN = 5;
const int MOTOR_DIR_PIN = 8;
const bool MOTOR_FEEDBACK_INVERT = true;
const bool DEBUG_FEEDBACK_SERIAL = true;
const float SERIAL_ANGLE_FILTER_ALPHA = 0.4;
const float SERIAL_ANGLE_HOLD_DEADBAND_DEG = 0.15;
const unsigned long SERIAL_STREAM_PERIOD_MS = 30;
const float GRIPPER_ANGLE_MAX_DEG = 360.0;

float targetWallAngleDeg = -1.0;
float targetWallStiffnessNPerM = 2000.0;
float targetWallForceGain = 1.0;
int targetWallContactMinPwm = 0;
unsigned long lastFeedbackPacketMillis = 0;
const unsigned long FEEDBACK_TIMEOUT_MS = 200;
const float VIRTUAL_WALL_DISABLED_DEG = -1.0;
const double HANDLE_RADIUS_M = 0.130;
const double MOTOR_PULLEY_RADIUS_M = 0.005;
const double SECTOR_PULLEY_RADIUS_M = 0.075;
const double DEFAULT_VIRTUAL_WALL_STIFFNESS_N_PER_M = 2000.0;
const double VIRTUAL_WALL_MAX_FORCE_N = 9999999999999999.0;
const double MOTOR_MAX_TORQUE_EST = 0.0183;
const double VIRTUAL_WALL_DUTY_EXPONENT = 1.20;
// "Virtual wall thickness": allow more penetration so squeezing further
// produces a larger force, but keep enough headroom to preserve softness
// differences between blocks.
const float VIRTUAL_WALL_MAX_PENETRATION_DEG = 30.0;  // was 20.0, then 45.0
const float VIRTUAL_WALL_MAX_DUTY = 0.92;             // was 0.90, then 0.98
const float VIRTUAL_WALL_CONTACT_ENTER_DEG = 2.2;
const float VIRTUAL_WALL_CONTACT_RELEASE_DEG = 0.5;
const float VIRTUAL_WALL_FORCE_FILTER_ALPHA = 0.10;
bool packetActive = false;
char packetBuffer[32];
uint8_t packetIndex = 0;
float previousWrappedGripperAngleDeg = 0.0;
float continuousGripperAngleDeg = 0.0;
bool gripperAngleInitialized = false;
bool suppressFeedbackWhileOpening = false;
bool lastMotorForward = true;
float filteredSerialAngleDeg = 0.0;
bool filteredSerialAngleInitialized = false;
bool virtualWallInContact = false;
double filteredWallForceN = 0.0;
unsigned long lastFeedbackDebugMillis = 0;

// ---------------- Encoder variables ----------------
long count1 = 0;
long count2 = 0;

uint8_t lastState1 = 0;
uint8_t lastState2 = 0;

unsigned long lastPrint = 0;

// quadrature lookup
int8_t quadTable[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

// ---------------- Robot parameters ----------------
const float CPR = 2400.0;
const float L1  = 18.0;
const float L2  = 18.0;

const long offset1 = 240;
const long offset2 = 480;

// ---------------- AS5600 Calibration Parameters ----------------
// Step 1: Fully close the gripper, read the raw_angle from the Serial Monitor, and put it here.
float CLOSED_OFFSET = 157;

// Step 2: Fully open the gripper and measure how many sensor degrees the AS5600
// travels from fully closed to fully open. This value can be larger than 360.
// The reported gripper angle will then be scaled so:
//   fully closed -> 0 deg
//   fully open   -> 360 deg
float GRIPPER_OPEN_TRAVEL_SENSOR_DEG = 500.0;

// Step 3: If the reported angle decreases while opening, change this to -1.0.
float ANGLE_DIRECTION = 1.0; 

// -------------------------------------------------------------------------------------

void setVirtualWallTarget(float wallAngleDeg, float wallStiffnessNPerM, float wallForceGain, int wallContactMinPwm) {
  targetWallAngleDeg = wallAngleDeg;
  targetWallStiffnessNPerM = wallStiffnessNPerM;
  targetWallForceGain = wallForceGain;
  targetWallContactMinPwm = wallContactMinPwm;
  lastFeedbackPacketMillis = millis();
}

float shortestAngleDeltaDeg(float targetDeg, float referenceDeg) {
  float delta = targetDeg - referenceDeg;
  while (delta > 180.0) {
    delta -= 360.0;
  }
  while (delta < -180.0) {
    delta += 360.0;
  }
  return delta;
}

float wrapAngle360(float angleDeg) {
  while (angleDeg < 0.0) {
    angleDeg += 360.0;
  }
  while (angleDeg >= 360.0) {
    angleDeg -= 360.0;
  }
  return angleDeg;
}

void updateMotorFeedbackOutput(float gripperAngleDeg) {
  float wallAngleDeg = targetWallAngleDeg;
  float wallStiffnessNPerM = targetWallStiffnessNPerM;
  float wallForceGain = targetWallForceGain;
  int wallContactMinPwm = targetWallContactMinPwm;
  if (millis() - lastFeedbackPacketMillis > FEEDBACK_TIMEOUT_MS) {
    wallAngleDeg = VIRTUAL_WALL_DISABLED_DEG;
    wallStiffnessNPerM = 0.0;
    wallForceGain = 0.0;
    wallContactMinPwm = 0;
  }

  double desiredForce = 0.0;
  double penetrationDeg = 0.0;
  if (wallAngleDeg >= 0.0) {
    penetrationDeg = wallAngleDeg - gripperAngleDeg;

    // Use contact hysteresis so noise near the wall does not repeatedly switch
    // the contact state on and off at the block boundary.
    if (!virtualWallInContact && penetrationDeg > VIRTUAL_WALL_CONTACT_ENTER_DEG) {
      virtualWallInContact = true;
    } else if (virtualWallInContact && penetrationDeg < VIRTUAL_WALL_CONTACT_RELEASE_DEG) {
      virtualWallInContact = false;
    }

    if (virtualWallInContact) {
      double effectivePenDeg = penetrationDeg - VIRTUAL_WALL_CONTACT_RELEASE_DEG;
      if (effectivePenDeg < 0.0) {
        effectivePenDeg = 0.0;
      }
      if (effectivePenDeg > VIRTUAL_WALL_MAX_PENETRATION_DEG) {
        effectivePenDeg = VIRTUAL_WALL_MAX_PENETRATION_DEG;
      }
      double penetrationM = HANDLE_RADIUS_M * (effectivePenDeg * M_PI / 180.0);
      if (penetrationM < 0.0) {
        penetrationM = 0.0;
      }
      double activeWallStiffness = wallStiffnessNPerM > 0.0 ? wallStiffnessNPerM : DEFAULT_VIRTUAL_WALL_STIFFNESS_N_PER_M;
      double activeWallForceGain = wallForceGain > 0.0 ? wallForceGain : 1.0;
      desiredForce = activeWallStiffness * penetrationM * activeWallForceGain;
      if (desiredForce < -VIRTUAL_WALL_MAX_FORCE_N) desiredForce = -VIRTUAL_WALL_MAX_FORCE_N;
      if (desiredForce >  VIRTUAL_WALL_MAX_FORCE_N) desiredForce =  VIRTUAL_WALL_MAX_FORCE_N;
    }
  } else {
    virtualWallInContact = false;
  }

  filteredWallForceN += VIRTUAL_WALL_FORCE_FILTER_ALPHA * (desiredForce - filteredWallForceN);
  if (fabs(filteredWallForceN) < 0.02) {
    filteredWallForceN = 0.0;
  }
  double force = filteredWallForceN;

  const double J = (HANDLE_RADIUS_M * MOTOR_PULLEY_RADIUS_M) / SECTOR_PULLEY_RADIUS_M;
  double Tp = J * force;
  double normalizedTorque = fabs(Tp) / MOTOR_MAX_TORQUE_EST;
  if (normalizedTorque < 0.0) {
    normalizedTorque = 0.0;
  }
  double duty = pow(normalizedTorque, VIRTUAL_WALL_DUTY_EXPONENT);
  if (duty > VIRTUAL_WALL_MAX_DUTY) {
    duty = VIRTUAL_WALL_MAX_DUTY;
  }
  if (duty > 1.0) {
    duty = 1.0;
  } else if (duty < 0.0) {
    duty = 0.0;
  }
  if (virtualWallInContact && wallAngleDeg >= 0.0) {
    int minPwm = max(0, min(255, wallContactMinPwm));
    duty = max(duty, minPwm / 255.0);
  }

  int outputPwm = (int)(duty * 255.0);
  if (DEBUG_FEEDBACK_SERIAL && (millis() - lastFeedbackDebugMillis >= 200)) {
    lastFeedbackDebugMillis = millis();
    Serial.print("[FBDBG] wall_deg=");
    Serial.print(wallAngleDeg, 2);
    Serial.print(" k=");
    Serial.print(wallStiffnessNPerM, 1);
    Serial.print(" gain=");
    Serial.print(wallForceGain, 2);
    Serial.print(" min_pwm=");
    Serial.print(wallContactMinPwm);
    Serial.print(" contact=");
    Serial.print(virtualWallInContact ? 1 : 0);
    Serial.print(" pen_deg=");
    Serial.print(penetrationDeg, 2);
    Serial.print(" forceN=");
    Serial.print(force, 3);
    Serial.print(" duty=");
    Serial.print(duty, 3);
    Serial.print(" out=");
    Serial.println(outputPwm);
  }

  bool forward = (force > 0.0);
  if (MOTOR_FEEDBACK_INVERT) {
    forward = !forward;
  }
  lastMotorForward = forward;
  digitalWrite(MOTOR_DIR_PIN, forward ? HIGH : LOW);
  analogWrite(MOTOR_PWM_PIN, outputPwm);
}

void processIncomingFeedback() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '<') {
      packetActive = true;
      packetIndex = 0;
      continue;
    }

    if (!packetActive) {
      continue;
    }

    if (c == '>') {
      packetBuffer[packetIndex] = '\0';
      float wallAngleDeg = VIRTUAL_WALL_DISABLED_DEG;
      float wallStiffnessNPerM = DEFAULT_VIRTUAL_WALL_STIFFNESS_N_PER_M;
      float wallForceGain = 1.0;
      int wallContactMinPwm = 0;
      char *firstComma = strchr(packetBuffer, ',');
      if (firstComma != nullptr) {
        *firstComma = '\0';
        wallAngleDeg = atof(packetBuffer);
        char *secondField = firstComma + 1;
        char *secondComma = strchr(secondField, ',');
        if (secondComma != nullptr) {
          *secondComma = '\0';
          wallStiffnessNPerM = atof(secondField);
          char *thirdField = secondComma + 1;
          char *thirdComma = strchr(thirdField, ',');
          if (thirdComma != nullptr) {
            *thirdComma = '\0';
            wallForceGain = atof(thirdField);
            wallContactMinPwm = atoi(thirdComma + 1);
          } else {
            wallForceGain = atof(thirdField);
          }
        } else {
          wallStiffnessNPerM = atof(secondField);
        }
      } else {
        wallAngleDeg = atof(packetBuffer);
      }
      setVirtualWallTarget(wallAngleDeg, wallStiffnessNPerM, wallForceGain, wallContactMinPwm);
      packetActive = false;
      packetIndex = 0;
      continue;
    }

    if (packetIndex < sizeof(packetBuffer) - 1) {
      packetBuffer[packetIndex++] = c;
    }
  }
}

void setup() {

  Serial.begin(115200);
  Wire.begin();
  if (MOTOR_PWM_PIN == 5 || MOTOR_PWM_PIN == 6) {
    // Pins 5/6 use Timer0 on AVR boards. Changing Timer0 also changes millis(),
    // which this sketch uses for serial pacing and feedback timeout logic.
    // Keep the default PWM frequency here to avoid unstable feedback behavior.
  } else {
    setPwmFrequency(MOTOR_PWM_PIN, 1);
  }

  pinMode(6, INPUT_PULLUP);   // Joint1 A
  pinMode(7, INPUT_PULLUP);   // Joint1 B

  pinMode(A0, INPUT_PULLUP);  // Joint2 A
  pinMode(A1, INPUT_PULLUP);  // Joint2 B

  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  setVirtualWallTarget(VIRTUAL_WALL_DISABLED_DEG, 0.0, 0.0, 0);
  updateMotorFeedbackOutput(0.0);

  // read initial state joint1
  uint8_t a1 = (PIND >> PD6) & 0x01;
  uint8_t b1 = (PIND >> PD7) & 0x01;
  lastState1 = (a1 << 1) | b1;

  // read initial state joint2
  uint8_t a2 = (PINC >> PC0) & 0x01;
  uint8_t b2 = (PINC >> PC1) & 0x01;
  lastState2 = (a2 << 1) | b2;
}

void loop() {
  processIncomingFeedback();
  if (filteredSerialAngleInitialized) {
    updateMotorFeedbackOutput(filteredSerialAngleDeg);
  } else {
    updateMotorFeedbackOutput(0.0);
  }
  

  // ---------- Joint1 ----------
  uint8_t a1 = (PIND >> PD6) & 0x01;
  uint8_t b1 = (PIND >> PD7) & 0x01;
  uint8_t currState1 = (a1 << 1) | b1;

  uint8_t index1 = (lastState1 << 2) | currState1;
  count1 += quadTable[index1];
  lastState1 = currState1;

  // ---------- Joint2 ----------
  uint8_t a2 = (PINC >> PC0) & 0x01;
  uint8_t b2 = (PINC >> PC1) & 0x01;
  uint8_t currState2 = (a2 << 1) | b2;

  uint8_t index2 = (lastState2 << 2) | currState2;
  count2 += quadTable[index2];
  lastState2 = currState2;

  // ---------- stream position and angle at a higher rate ----------
  if (millis() - lastPrint > SERIAL_STREAM_PERIOD_MS) {

    lastPrint = millis();

    // -------- convert to angles --------
    float theta1 = (count1 + offset1) * 2.0 * PI / CPR;
    float theta2 = -(count2 + offset2) * 2.0 * PI / CPR;

    // -------- Forward Kinematics --------
    float x = L1 * cos(theta1) + L2 * cos(theta1 + theta2) - 28.78;
    float z = L1 * sin(theta1) + L2 * sin(theta1 + theta2);

    x = -x;

    // -------- Read AS5600 --------
    Wire.beginTransmission(AS5600_I2C_ADDRESS);
    Wire.write(0x0E);
    Wire.endTransmission();
    Wire.requestFrom(AS5600_I2C_ADDRESS, 2);

    int angleValue = ((Wire.read() << 8) | Wire.read()) & 0x0FFF;
    float raw_angle = angleValue * 360.0 / 4096.0;

    // -------- Calibrate Angle (Continuous 0-360 logic) --------
    // 1. Subtract the zero offset
    float diff = raw_angle - CLOSED_OFFSET;
    
    // 2. Apply direction FIRST
    diff = diff * ANGLE_DIRECTION;
    
    // 3. Wrap into a strictly positive 0 to 360 range
    while (diff < 0.0) {
      diff += 360.0;
    }
    while (diff >= 360.0) {
      diff -= 360.0;
    }
    
    float wrapped_angle = diff;
    float final_angle = wrapped_angle;

    // 4. Track the calibrated angle continuously across the AS5600 wrap.
    // The internal angle may exceed 360 deg, but the exported angle is clamped
    // so the virtual gripper stays fully open instead of snapping back to 0.
    if (!gripperAngleInitialized) {
      previousWrappedGripperAngleDeg = wrapped_angle;
      continuousGripperAngleDeg = wrapped_angle;
      gripperAngleInitialized = true;
      suppressFeedbackWhileOpening = false;
    } else {
      float wrapped_delta = shortestAngleDeltaDeg(wrapped_angle, previousWrappedGripperAngleDeg);
      continuousGripperAngleDeg += wrapped_delta;
      if (continuousGripperAngleDeg < 0.0) {
        continuousGripperAngleDeg = 0.0;
      }
      previousWrappedGripperAngleDeg = wrapped_angle;
      suppressFeedbackWhileOpening = false;
    }
    float scaled_angle = continuousGripperAngleDeg * (GRIPPER_ANGLE_MAX_DEG / max(GRIPPER_OPEN_TRAVEL_SENSOR_DEG, 1.0f));
    final_angle = constrain(scaled_angle, 0.0, GRIPPER_ANGLE_MAX_DEG);

    if (!filteredSerialAngleInitialized) {
      filteredSerialAngleDeg = final_angle;
      filteredSerialAngleInitialized = true;
    } else {
      float delta = final_angle - filteredSerialAngleDeg;
      if (fabs(delta) > SERIAL_ANGLE_HOLD_DEADBAND_DEG) {
        filteredSerialAngleDeg += SERIAL_ANGLE_FILTER_ALPHA * delta;
      }
      filteredSerialAngleDeg = constrain(filteredSerialAngleDeg, 0.0, GRIPPER_ANGLE_MAX_DEG);
    }

    // ---------- Output ----------
    Serial.print(x);
    Serial.print(",");
    Serial.print(z);
    Serial.print(",");
    
    Serial.println(filteredSerialAngleDeg);

  }
}

// --------------------------------------------------------------
// Function to set PWM Freq -- based on Hapkit template
// --------------------------------------------------------------
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}