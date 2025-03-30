#include <serialize.h>
#include <stdarg.h>
#include <math.h>

#include "packet.h"
#include "constants.h"

bool MANUAL_MODE = false;
volatile TDirection dir = STOP;

volatile float distance = DIST_MID;
unsigned long targetDist;

volatile float angleDur = ANG_MID;
unsigned long lastTurnTime;

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
#define PI                  3.141592654

// Alex's length and breadth in cm. You must measure
// and substitute with the correct values.
#define ALEX_LENGTH         16
#define ALEX_BREADTH        6

// Alex's Diagonal. We compute and store this once since
// it is expensive to compute and never chanegs.
float ALEX_DIAGONAL = 0.0;

// Alex's turning circumference, calculated once. We
// assume that Alex "turns on a dime"
float ALEX_CIRC = 0.0;

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
// unsigned long deltaDist; // The distance the robot should move.
// unsigned long newDist; // The target distance the robot should reach before stopping

// Variables to keep track of our turning angle
// unsigned long deltaTicks;
// unsigned long targetTicks;

void setup() {
  // put your setup code here, to run once:
  // Calculate the constants for turning angles
  ALEX_DIAGONAL = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  ALEX_CIRC = PI * ALEX_DIAGONAL;

  cli();
  setupEINT();
  setupSerial();
  startSerial();

  setupMotors();
  startMotors();

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
        if (MANUAL_MODE) {
          distance = (double) command->params[0];
        }
        forward();
      break;

    // Reverse movement
    case COMMAND_REVERSE:
        if (MANUAL_MODE) {
          distance = (double) command->params[0];
        }
        backward();
      break;

    case COMMAND_TURN_LEFT:
        if (MANUAL_MODE) {
          angleDur = (double) command->params[0];
        }
        left();
      break;

    case COMMAND_TURN_RIGHT:
        if (MANUAL_MODE) {
          angleDur = (double) command->params[0];
        }
        right();
      break;

    // Stop movement
    case COMMAND_STOP:
        sendOK();
        stop();
      break;

    case COMMAND_SPEED_SLOW:
        distance = DIST_SHORT;
        angleDur = ANG_SHORT;
        sendOK();
      break;

    case COMMAND_SPEED_MID:
        distance = DIST_MID;
        angleDur = ANG_MID;
        sendOK();
      break;

    case COMMAND_SPEED_FAST:
        distance = DIST_FAR;
        angleDur = ANG_FAR;
        sendOK();
      break;

    // Get stats (send back status)
    case COMMAND_GET_STATS:
        sendStatus();
      break;

    // Clear stats (clear counters based on the parameter)
    case COMMAND_CLEAR_STATS:
        clearOneCounter(command->params[0]); // Clear the specific counter
        sendOK();  // Send an OK response to acknowledge the command
      break;

    case COMMAND_MANUAL:
        MANUAL_MODE = !MANUAL_MODE; // Toggle manual mode
        if (!MANUAL_MODE) { // Reset distance and angle when toggle back to auto mode
          distance = DIST_MID;
          angleDur = ANG_MID;
        }
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

#define STOP_DELAY 20000 // Milliseconds
void loop() {
  if (dir == FORWARD) {
    if (forwardDist >= targetDist) {
      targetDist = 0;
      stop();
      delay(STOP_DELAY);
    }
  } else if (dir == BACKWARD) {
    if (reverseDist >= targetDist) {
      targetDist = 0;
      stop();
      delay(STOP_DELAY);
    }
  } else if (dir == RIGHT or dir == LEFT) {
    if (millis() - lastTurnTime >= angleDur) {
      stop();
      delay(STOP_DELAY);
    }
  }
  
  // Handle packet
  TPacket recvPacket; // This holds commands from the Pi
  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK) {
    handlePacket(&recvPacket);
  } else if (result == PACKET_BAD) {
    sendBadPacket();
  } else if (result == PACKET_CHECKSUM_BAD) {
    sendBadChecksum();
  }
}
