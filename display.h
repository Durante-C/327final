/*
 * display.h
 *
 *  Requires the use of UCA0 in UART mode. Functions at a baud rate of 9600,
 *  and uses the transmit interrupt to work its way through the sent message.
 *
 *  Created on: Apr 10, 2022
 *      Author: Nicolas Rossi
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdbool.h>
#include <msp430g2553.h>

#define WAIT_DISPLAY while(!donePrinting())
#define OFF 0
#define WHITE 1
#define RED 2
#define GREEN 3
#define BLUE 4

extern volatile int wakeup_flag;

// Initialize everything required for the display.
void initDisplay(void);

// Print the given number of bytes to the display.
void printBytes(char *, unsigned int);

void set_display_color(int color);

// Returns true if the current print has finished.
bool donePrinting(void);

void clearDisplay(void);

#endif /* DISPLAY_H_ */
