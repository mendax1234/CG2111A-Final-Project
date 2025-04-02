#include <Servo.h>
Servo rightArm;
Servo leftArm;

void setupArm() {
  rightArm.attach(16);
  leftArm.attach(15);

  rightArm.write(90);
  leftArm.write(0);
}

void openArm() {
  rightArm.write(90);
  leftArm.write(0);
}

void closeArm() {
  rightArm.write(45);
  leftArm.write(45);
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
