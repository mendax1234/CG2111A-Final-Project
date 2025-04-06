#include <AFMotor.h>

// Motor pin definitions
#define FRONT_LEFT   4 // M4 on the driver shield
#define FRONT_RIGHT  1 // M1 on the driver shield
#define BACK_LEFT    3 // M3 on the driver shield
#define BACK_RIGHT   2 // M2 on the driver shield

// Motor objects
AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

// Direction control functions for left motors
void leftMotorsForward() {
  motorFL.run(FORWARD);
  motorBL.run(BACKWARD);
}

void leftMotorsReverse() {
  motorFL.run(BACKWARD);
  motorBL.run(FORWARD);
}

// Direction control functions for right motors
void rightMotorsForward() {
  motorFR.run(FORWARD);
  motorBR.run(BACKWARD);
}

void rightMotorsReverse() {
  motorFR.run(BACKWARD);
  motorBR.run(FORWARD);
}

// Set speed for all motors (speed in percentage, 0-100)
void setMotorSpeed(float speed) {
  int speed_scaled = (speed / 100.0) * 255; // Scale to 0-255
  if (speed_scaled < 0) speed_scaled = 0;
  if (speed_scaled > 255) speed_scaled = 255;
  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);
}

// Distance-based
// Movement functions
void forward(float dist, float speed) {
  if (dist > 0) {
    deltaDist = dist;
  } else {
    deltaDist = 9999999; // Move indefinitely
  }
  newDist = forwardDist + deltaDist;
  dir = FORWARD;
  setMotorSpeed(speed);
  leftMotorsForward();
  rightMotorsForward();
}

void backward(float dist, float speed) {
  if (dist > 0) {
    deltaDist = dist;
  } else {
    deltaDist = 9999999; // Move indefinitely
  }
  newDist = reverseDist + deltaDist;
  dir = (TDirection) BACKWARD;
  setMotorSpeed(speed);
  leftMotorsReverse();
  rightMotorsReverse();
}

// Time-based
void forwardTime(float time, float speed) {
  dir = (TDirection) FORWARD;
  setMotorSpeed(speed);
  leftMotorsForward();
  rightMotorsForward();
  delay(time*10); // Run for the specified time in milliseconds
  stop();
}

void backwardTime(float time, float speed) {
  dir = (TDirection) BACKWARD;
  setMotorSpeed(speed);
  leftMotorsReverse();
  rightMotorsReverse();
  delay(time*10); // Run for the specified time in milliseconds
  stop();
}

void leftTime(float time, float speed) {
  dir = (TDirection) LEFT;
  setMotorSpeed(speed);
  leftMotorsForward();
  rightMotorsReverse();
  delay(time*10); // Run for the specified time in milliseconds
  stop();
}

void rightTime(float time, float speed) {
  dir = (TDirection) RIGHT;
  setMotorSpeed(speed);
  leftMotorsReverse();
  rightMotorsForward();
  delay(time*10); // Run for the specified time in milliseconds
  stop();
}

void left(float ang, float speed) {
  if (ang == 0) {
    deltaTicks = 99999999; // Turn indefinitely
  } else {
    deltaTicks = computeDeltaTicks(ang); // Assumes this function exists elsewhere
  }
  targetTicks = leftReverseTicksTurns + deltaTicks;
  dir = (TDirection) LEFT;
  setMotorSpeed(speed);
  leftMotorsForward();
  rightMotorsReverse();
}

void right(float ang, float speed) {
  if (ang == 0) {
    deltaTicks = 99999999; // Turn indefinitely
  } else {
    deltaTicks = computeDeltaTicks(ang); // Assumes this function exists elsewhere
  }
  targetTicks = rightReverseTicksTurns + deltaTicks;
  dir = (TDirection) RIGHT;
  setMotorSpeed(speed);
  leftMotorsReverse();
  rightMotorsForward();
}

void stop() {
  dir = (TDirection) STOP;
  motorFL.run(RELEASE);
  motorFR.run(RELEASE);
  motorBL.run(RELEASE);
  motorBR.run(RELEASE);
}
