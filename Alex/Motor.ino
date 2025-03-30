#include <AFMotor.h>

// Motor control
#define FRONT_LEFT   1 // M3 on the driver shield
#define FRONT_RIGHT  4 // M2 on the driver shield
#define BACK_LEFT    2 // M4 on the driver shield
#define BACK_RIGHT   3 // M1 on the driver shield

AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

// void move(float speed, int direction)
// {
//   setMotorSpeed(speed);

//   switch(direction)
//     {
//       case BACKWARD:
//         motorFL.run(BACKWARD);
//         motorFR.run(BACKWARD);
//         motorBL.run(FORWARD);
//         motorBR.run(FORWARD);
//       break;
//       case FORWARD:
//         motorFL.run(FORWARD);
//         motorFR.run(FORWARD);
//         motorBL.run(BACKWARD);
//         motorBR.run(BACKWARD);
//       break;
//       case LEFT:
//         motorFL.run(BACKWARD);
//         motorFR.run(FORWARD);
//         motorBL.run(FORWARD);
//         motorBR.run(BACKWARD);
//       break;
//       case RIGHT:
//         motorFL.run(FORWARD);
//         motorFR.run(BACKWARD);
//         motorBL.run(BACKWARD);
//         motorBR.run(FORWARD); 
//       break;
//       case STOP:
//       default:
//         motorFL.run(STOP);
//         motorFR.run(STOP);
//         motorBL.run(STOP);
//         motorBR.run(STOP); 
//     }
// }

void setupMotors() {
  // Prepare for bare-metal usage
}

void startMotors() {
  // Prepare for bare-metal usage
}

void stopMotors() {
  motorFL.run(STOP);
  motorFR.run(STOP);
  motorBL.run(STOP);
  motorBR.run(STOP); 
}

void leftMotorsForward() {
  motorFL.run(FORWARD);
  motorBL.run(BACKWARD);
}

void leftMotorsReverse() {
  motorFL.run(BACKWARD);
  motorBL.run(FORWARD);
}

void rightMotorsForward() {
  motorFR.run(FORWARD);
  motorBR.run(BACKWARD);
}

void rightMotorsReverse() {
  motorFR.run(BACKWARD);
  motorBR.run(FORWARD);
}

void setMotorSpeed(float speed) {
  // Map into reasonable range
  if (speed < 0) {
    speed = 0;
  } else if (speed > 100.0) {
    speed = 100.0;
  }

  int speed_scaled = (int) ((speed / 100.0) * 255.0);

  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);
}

void forward()
{
  dir = FORWARD;
  setMotorSpeed(SPEED_FAST);

  targetDist = forwardDist + distance;

  leftMotorsForward();
  rightMotorsForward();
}

void backward() {
  dir = BACKWARD;
  setMotorSpeed(SPEED_FAST);

  targetDist = reverseDist + distance;

  leftMotorsReverse();
  rightMotorsReverse();
}

void left() {
  dir = LEFT;
  setMotorSpeed(SPEED_FAST);

  // Angle-control mode
  //  unsigned long deltaTicks = (angle / 360.0) * (ALEX_CIRC / WHEEL_CIRC) * COUNTS_PER_REV;
  //  targetTurnTicks = leftReverseTicks + deltaTicks;
  //  dbprintf("Left ticks: %d\n", deltaTicks);
  lastTurnTime = millis();

  leftMotorsReverse();
  rightMotorsForward();
}

void right() {
  dir = RIGHT;
  setMotorSpeed(SPEED_FAST);

  // Angle-control mode
  //  unsigned long deltaTicks = (angle / 360.0) * (ALEX_CIRC / WHEEL_CIRC) * COUNTS_PER_REV;
  //  targetTurnTicks = rightReverseTicks + deltaTicks;
  //  dbprintf("Right ticks: %d\n", deltaTicks);
  lastTurnTime = millis();

  leftMotorsForward();
  rightMotorsReverse();
}

void stop()
{
  dir = STOP;
  stopMotors();
}
