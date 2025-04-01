#include <Servo.h>
Servo leftArm;
Servo rightArm;

void setupArm() {
  leftArm.attach(16);
  rightArm.attach(15);

  // Set initial positions
  leftArm.write(0);    // Set left servo to 0 degrees
  rightArm.write(90);  // Set right servo to 90 degrees
}

// Process color readings and control servos
void processColor(TColorType colorType) {
  // Determine color based on RGB frequencies
  switch (colorType) {
    case WHITE:
      leftArm.write(90); // Open mechanism
      rightArm.write(0);
      break;
      
    case RED:
      if (redFreq < blueFreq && redFreq <= greenFreq && redFreq < 23) {
        leftArm.write(45); // Close mechanism
        rightArm.write(45);
      }
      break;
      
    case BLUE:
      if (blueFreq < greenFreq && blueFreq < redFreq && blueFreq < 20) {
        leftArm.write(90); // Open mechanism
        rightArm.write(0);
      }
      break;
      
    case GREEN:
      if (greenFreq < redFreq && (greenFreq - blueFreq) <= 8) {
        leftArm.write(90); // Open mechanism
        rightArm.write(0);
      }
      break;
      
    case UNKNOWN:
    default:
      leftArm.write(90); // Open mechanism
      rightArm.write(0);
      break;
  }
}

