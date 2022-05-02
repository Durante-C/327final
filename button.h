/*
 * button.h
 *
 *  Created on: Apr 20, 2022
 *      Author: ryanpai
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#define SW1_MASK BIT0
#define SW2_MASK BIT1
#define SW3_MASK BIT2
#define SW4_MASK BIT3

#define UP_BTN_MASK SW2_MASK
#define DN_BTN_MASK SW1_MASK
#define SL_BTN_MASK SW4_MASK
#define BK_BTN_MASK SW3_MASK

#define ALL_SW_MASK 0xF

#define UP_WAKEUP 1
#define DN_WAKEUP 2
#define SL_WAKEUP 3
#define BK_WAKEUP 4
#define WDT_WAKEUP 5

extern volatile int wakeup_flag;
extern volatile int wdt_cycle_counter;

void switch_setup();

#endif /* BUTTON_H_ */
