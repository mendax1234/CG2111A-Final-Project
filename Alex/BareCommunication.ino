/*
 * Setup and start codes for serial communications
 *
 */
#include "packet.h"
#include "serialize.h"
#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

#define MAX_BUFFER_LEN 1024

void setupSerial() {
  UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
  UBRR0L = (unsigned char)(UBRR_VALUE);

  UCSR0B = 0b00011000; // RXEN0 and TXEN0
  UCSR0C = 0b00000110; // UCSZ01 and UCSZ00 for 8-bit, no parity
}

void startSerial() {
  // setupSerial already start serial connection
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid.
// This will be replaced later with bare-metal code.

int readSerial(char *buffer) {
  int count = 0;

  while (UCSR0A & 0b10000000) { // RXC0: Receive Complete
    buffer[count++] = UDR0;
    if (count >= MAX_BUFFER_LEN)
      break;
  }
  return count;
}

void writeSerial(const char *buffer, int len) {
  for (int i = 0; i < len; ++i) {
    while (!(UCSR0A & 0b00100000))
      ; // Wait until UDRE0: Data Register Empty
    UDR0 = buffer[i];
  }
}

/*
 *
 * Alex Communication Routines.
 *
 */

TResult readPacket(TPacket *packet) {
  // Reads in data from the serial port and
  // deserializes it.Returns deserialized
  // data in "packet".

  char buffer[PACKET_SIZE];
  int len;

  len = readSerial(buffer);

  if (len == 0)
    return PACKET_INCOMPLETE;
  else
    return deserialize(buffer, len, packet);
}

void sendResponse(TPacket *packet) {
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;

  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}