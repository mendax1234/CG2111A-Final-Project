#include <Servo.h>
Servo rightArm;
Servo leftArm;

void setupArm() {
  rightArm.attach(16);
  leftArm.attach(15);

  rightArm.write(90);
  leftArm.write(0);
}

// Process color readings and control servos
void processColor(TColorType colorType) {
  // Determine color based on RGB frequencies
  switch (colorType) {
    case WHITE:
      rightArm.write(90); // Open mechanism
      leftArm.write(0);
      break;
      
    case RED:
      if (redFreq < blueFreq && redFreq <= greenFreq && redFreq < 23) {
        rightArm.write(45); // Close mechanism
        leftArm.write(45);
      }
      break;
      
    case BLUE:
      if (blueFreq < greenFreq && blueFreq < redFreq && blueFreq < 20) {
        rightArm.write(90); // Open mechanism
        leftArm.write(0);
      }
      break;
      
    case GREEN:
      if (greenFreq < redFreq && (greenFreq - blueFreq) <= 8) {
        rightArm.write(90); // Open mechanism
        leftArm.write(0);
      }
      break;
      
    case UNKNOWN:
    default:
      rightArm.write(90); // Open mechanism
      leftArm.write(0);
      break;
  }
}

