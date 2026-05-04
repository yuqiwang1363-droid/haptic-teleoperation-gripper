long count1 = 0;
long count2 = 0;

uint8_t lastState1 = 0;
uint8_t lastState2 = 0;

unsigned long lastPrint = 0;

int8_t quadTable[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

void setup() {
  Serial.begin(115200);

  pinMode(8, INPUT_PULLUP);   // Joint1 A
  pinMode(9, INPUT_PULLUP);   // Joint1 B
  pinMode(A0, INPUT_PULLUP);  // Joint2 A
  pinMode(A1, INPUT_PULLUP);  // Joint2 B

  // initial state
  uint8_t a1 = (PINB >> PB0) & 0x01;   // D8
  uint8_t b1 = (PINB >> PB1) & 0x01;   // D9
  lastState1 = (a1 << 1) | b1;

  uint8_t a2 = (PINC >> PC0) & 0x01;   // A0
  uint8_t b2 = (PINC >> PC1) & 0x01;   // A1
  lastState2 = (a2 << 1) | b2;
}

void loop() {
  // -------- Joint1 --------
  uint8_t a1 = (PINB >> PB0) & 0x01;   // D8
  uint8_t b1 = (PINB >> PB1) & 0x01;   // D9
  uint8_t currState1 = (a1 << 1) | b1;

  uint8_t index1 = (lastState1 << 2) | currState1;
  count1 += quadTable[index1];
  lastState1 = currState1;

  // -------- Joint2 --------
  uint8_t a2 = (PINC >> PC0) & 0x01;   // A0
  uint8_t b2 = (PINC >> PC1) & 0x01;   // A1
  uint8_t currState2 = (a2 << 1) | b2;

  uint8_t index2 = (lastState2 << 2) | currState2;
  count2 += quadTable[index2];
  lastState2 = currState2;

  // print slower
  if (millis() - lastPrint > 100) {
    lastPrint = millis();
    Serial.print("Joint1: ");
    Serial.print(count1);
    Serial.print("   Joint2: ");
    Serial.println(count2);
  }
}