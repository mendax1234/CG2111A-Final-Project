void setupColor() {
  pinMode(S0, OUTPUT); // pin modes
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH); // Putting S0/S1 on HIGH/HIGH levels  means the output
                          // frequency scalling is at 100% (recommended)
  digitalWrite(S1, HIGH); // LOW/LOW is off HIGH/LOW is 20% and LOW/HIGH is  2%
}

int avgFREQ() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    total += pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
    delay(colorAverageDelay);
  }
  return total / 5;
}

void findColor() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(colorSensorDelay);
  redFreq = avgFREQ();

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(colorSensorDelay);
  greenFreq = avgFREQ();

  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(colorSensorDelay);
  blueFreq = avgFREQ();
}

void getColor() {
  // Determine color based on RGB frequencies
  // Case 1: Red
  if (redFreq < blueFreq && redFreq <= greenFreq && redFreq < 23) {
    detectedColor = RED;
  }
  // Case 2: Green
  else if (greenFreq < redFreq && (greenFreq - blueFreq) <= 8) {
    detectedColor = GREEN;
  }
  // Case 3: Unknown color - Default case if no other condition is met
  else {
    detectedColor = UNKNOWN;
  }
}

// Send color data via packet system
void sendColor(TColorType colorType) {
  TPacket colorPacket;
  colorPacket.packetType = PACKET_TYPE_RESPONSE;
  colorPacket.command = RESP_COLOR;

  colorPacket.params[0] = redFreq;
  colorPacket.params[1] = greenFreq;
  colorPacket.params[2] = blueFreq;
  colorPacket.params[3] = colorType;

  sendResponse(&colorPacket);
}
