void setupArm() {
  // Pin 45 and 46 are used instead due to lack of  PWM on pin 15 and 16
  DDRL |= 0b00011000;

  // Non-inverting PWM on OC5A and OC5B
  TCCR5A = 0b10100010;
  // Fast PWM, prescaler = 8
  TCCR5B = 0b00011010;

  ICR5 = 40000;
  OCR5A = 2000;
  OCR5B = 3000;
}

void openArm() {
  OCR5A = 2000;
  OCR5B = 3000;
}

void closeArm() {
  OCR5A = 2500;
  OCR5B = 2500;
}

// Process color readings and control servos
void processColor(TColorType colorType) {
  // Determine color based on RGB frequencies
  switch (colorType) {
  case WHITE:
    openArm();
    break;

  case RED:
    closeArm();
    break;

  case BLUE:
    openArm();
    break;

  case GREEN:
    closeArm();
    break;

  case UNKNOWN:
    openArm();
    break;

  default:
    openArm();
    break;
  }
}
