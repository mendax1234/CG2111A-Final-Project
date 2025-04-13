#include <serialize.h>
#include <stdarg.h>
#include <math.h>

#include "packet.h"
#include "constants.h"

/*
 * Alex's configuration constants
 */

// Number of ticks per revolution from the 
// wheel encoder.

#define COUNTS_PER_REV      4

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled 
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC          22

// PI, for calculating circumference
// #define PI                  3.141592654

// Alex's length and breadth in cm. You must measure
// and substitute with the correct values.
#define ALEX_LENGTH         27.9
#define ALEX_BREADTH        16.4

/*
      Alex's Colour Sensor variables
*/
#define S0 25        //Module pins  wiring
#define S1 27
#define S2 29
#define S3 31
#define sensorOut 33

int redFreq = 0, greenFreq = 0, blueFreq = 0;  // RGB frequency values
int colorSensorDelay = 20;  // Delay between readings
int colorAverageDelay = 5;  // Delay between averaging samples

volatile TColorType detectedColor = UNKNOWN;

// Alex's Control Mode
bool MANUAL = false;

// Alex's Direction enum state variable
volatile TDirection dir;

// Constant under Auto Mode
volatile float distance = DIST_MID;
volatile float angleDur = ANG_MID;
volatile float speed = SPEED_FAST;
unsigned long lastMoveDist;
unsigned long lastTurnTime;

// Alex's Diagonal. We compute and store this once since
// it is expensive to compute and never chanegs.
float alexDiagonal = 0.0;

// Alex's turning circumference, calculated once. We
// assume that Alex "turns on a dime"
float alexCirc = 0.0;

/*
 *    Alex's State Variables
 */

// Store the ticks from Alex's left and
// right encoders.
volatile unsigned long leftForwardTicks; 
volatile unsigned long rightForwardTicks;
volatile unsigned long leftReverseTicks; 
volatile unsigned long rightReverseTicks;

// Left and right encoder ticks for turning
volatile unsigned long leftForwardTicksTurns;
volatile unsigned long rightForwardTicksTurns;
volatile unsigned long leftReverseTicksTurns;
volatile unsigned long rightReverseTicksTurns;

// Store the revolutions on Alex's left
// and right wheels
volatile unsigned long leftRevs;
volatile unsigned long rightRevs;

// Forward and backward distance traveled
volatile unsigned long forwardDist;
volatile unsigned long reverseDist;

// Variables to keep track of whether we've moved
// a command distance
unsigned long deltaDist; // The distance the robot should move.
unsigned long newDist; // The target distance the robot should reach before stopping

// Variables to keep track of our turning angle
unsigned long deltaTicks;
unsigned long targetTicks;

void setup() {
  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  alexCirc = PI * alexDiagonal;
  cli();
  setupEINT();
  setupSerial();
  startSerial();

  // setupMotors();
  // startMotors();

  setupColor();
  setupArm();

  enablePullups();
  initializeState();
  sei();
}

void handleCommand(TPacket *command)
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        if (MANUAL) {
          forward((double) command->params[0], (float) command->params[1]);
        } else {
          forward(distance, speed);
        }
      break;

    // Reverse movement
    case COMMAND_REVERSE:
        sendOK();
        if (MANUAL) {
          backward((double) command->params[0], (float) command->params[1]);
        } else {
          backward(distance, speed);
        }
      break;

    // Turn left
    case COMMAND_TURN_LEFT:
        sendOK();
        if (MANUAL) {
          left((double) command->params[0], (float) command->params[1]);
        } else {
          left(angleDur, speed);
        }
      break;

    // Turn right
    case COMMAND_TURN_RIGHT:
        sendOK();
        if (MANUAL) {
          right((double) command->params[0], (float) command->params[1]);
        } else {
          right(angleDur, speed);
        }
      break;

    // Stop movement
    case COMMAND_STOP:
        sendOK();
        stop();
      break;

    case COMMAND_SLOW_MODE:
      speed = SPEED_FAST;
      distance = DIST_SHORT;
      angleDur = ANG_SHORT;
      sendOK();
      break;
    
    case COMMAND_NORMAL_MODE:
      speed = SPEED_FAST;
      distance = DIST_MID;
      angleDur = ANG_MID;
      sendOK();
      break;

    case COMMAND_FAST_MODE:
      speed = SPEED_FAST;
      distance = DIST_FAR;
      angleDur = ANG_FAR;
      sendOK();
      break;

    // Get stats (send back status)
    case COMMAND_GET_STATS:
        sendStatus();
        sendOK();
      break;

    // Clear stats (clear counters based on the parameter)
    case COMMAND_CLEAR_STATS:
        clearOneCounter(command->params[0]); // Clear the specific counter
        sendOK();  // Send an OK response to acknowledge the command
      break;

    case COMMAND_MANUAL:
      MANUAL = !MANUAL; // Toggle manual mode
      if (!MANUAL) { // Reset distance and angle when toggle back to auto mode
        distance = DIST_MID;
        angleDur = ANG_MID;
      }
      sendOK();
      break;

    case COMMAND_COLOR:
      findColor();
      getColor();
      sendColor(detectedColor);
      sendOK();
      break;

    case COMMAND_OPEN_ARM:
      openArm();
      sendOK();
      break;

    case COMMAND_CLOSE_ARM:
      closeArm();
      sendOK();
      break;

    case COMMAND_PRINT_INST:
      sendOK();
      break;
        
    default:
      sendBadCommand();
  }
}

void handlePacket(TPacket *packet)
{
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

void loop() {
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK) {
    handlePacket(&recvPacket);
  }
  else {
    if(result == PACKET_BAD) {
      sendBadPacket();
    } else {
      if(result == PACKET_CHECKSUM_BAD) {
        sendBadChecksum();
      }
    }
  }

  // WASD move
  if (distance > 0) {
    if ((dir == FORWARD && millis() - lastMoveDist >= distance) ||
        (dir == BACKWARD && millis() - lastMoveDist >= distance) ||
        (dir == (TDirection) STOP)) {
      // deltaDist = 0;
      // newDist = 0;
      stop();
    }
  } 
  
  if (angleDur > 0) {
    if ((dir == LEFT && millis() - lastTurnTime >= angleDur) ||
        (dir == RIGHT && millis() - lastTurnTime >= angleDur) ||
        (dir == (TDirection) STOP)) {
      // deltaTicks = 0;
      // targetTicks = 0;
      stop();
    }
  }
  /* 
  // Distance control
    if (deltaDist > 0) {
    if ((dir == FORWARD && forwardDist > newDist) ||
        (dir == BACKWARD && reverseDist > newDist) ||
        (dir == (TDirection) STOP)) {
      deltaDist = 0;
      newDist = 0;
      stop();
    }
  } 

  // Degree Control
  if (deltaTicks > 0) {
    if ((dir == LEFT && leftReverseTicksTurns >= targetTicks) ||
        (dir == RIGHT && rightReverseTicksTurns >= targetTicks) ||
        (dir == (TDirection) STOP)) {
      deltaTicks = 0;
      targetTicks = 0;
      stop();
    }
  }
  */
}
