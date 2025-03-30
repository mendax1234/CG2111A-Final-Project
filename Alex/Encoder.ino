/*
   Setup and start codes for external interrupts and
   pullup resistors.
*/

// Enable pull up resistors on pins 18 and 19
void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  // 19 and 18. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs.
  DDRD &= ~((1 << PD2) | (1 << PD3)); // Set PD2 and PD3 as input
  PORTD |= (1 << PD2) | (1 << PD3);   // Enable internal pull-ups
  
}

// Functions to be called by INT2 and INT3 ISRs.
void leftISR()
{
  if (dir == FORWARD) {
    leftForwardTicks++;
    forwardDist = (unsigned long) ((float) leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
    // dbprintf("Left Forward Ticks: %ld", leftForwardTicks);
    // dbprintf("Forward Distance: %ld", forwardDist);
  } else if (dir == BACKWARD) {
    leftReverseTicks++;
    reverseDist = (unsigned long) ((float) leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
    // dbprintf("Left Reverse Ticks: %ld", leftReverseTicks);
    // dbprintf("Reverse Distance: %ld", reverseDist);
  } else if (dir == LEFT) {
    leftReverseTicksTurns++;
    // dbprintf("Left Forward Tick Turn: %ld", leftForwardTicksTurns);
  } else if (dir == RIGHT) {
    leftForwardTicksTurns++;
    // sdbprintf("Left Reverse Tick Turn: %ld", leftReverseTicksTurns);
  }
}

void rightISR()
{
  if (dir == FORWARD) {
    rightForwardTicks++;
    // dbprintf("Right Forward Ticks: %ld", rightForwardTicks);
  } else if (dir == BACKWARD) {
    rightReverseTicks++;
    // dbprintf("Right Reverse Ticks: %ld", rightReverseTicks);
  } else if (dir == LEFT) {
    rightForwardTicksTurns++;
    // dbprintf("Right Forward Tick Turn: %ld", rightForwardTicksTurns);
  } else if (dir == RIGHT) {
    rightReverseTicksTurns++;
    // dbprintf("Right Reverse Tick Turn: %ld", rightReverseTicksTurns);
  }
}

// Set up the external interrupt pins INT2 and INT3
// for falling edge triggered. Use bare-metal.
void setupEINT()
{
  EICRA |= (1 << ISC31) | (1 << ISC21);  // Set INT3 and INT2 to falling edge
  EICRA &= ~((1 << ISC30) | (1 << ISC20)); 
  EIMSK |= (1 << INT3) | (1 << INT2); // Enable INT3 and INT2 interrupts
}

// Implement the external interrupt ISRs below.
// INT3 ISR should call leftISR while INT2 ISR
// should call rightISR.

ISR(INT2_vect)
{
  rightISR();  // Call the rightISR function
}

ISR(INT3_vect)
{
  leftISR();  // Call the leftISR function
}