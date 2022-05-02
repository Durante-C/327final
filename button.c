/*
 * button.c
 *
 *  Created on: Apr 20, 2022
 *      Author: ryanpai
 */

#include "msp430g2553.h"
#include "button.h"

void switch_setup() {
    P2DIR &= ~ALL_SW_MASK; // P2.0, P2.2, P2.3, P2.4 are inputs
    P2REN = ALL_SW_MASK; // enable pullup/pulldown resistor
    P2OUT |= ALL_SW_MASK; // set to pullup resistor

    P2IE |= ALL_SW_MASK; // enables interrupt for all switches
    P2IES |= ALL_SW_MASK; // sets to negative edge (high->low) for all switches
    P2IFG &= 0; // clears interrupt flag

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#endif
{
    wdt_cycle_counter = 0;

    if ((P2IFG & UP_BTN_MASK) == UP_BTN_MASK) {
        wakeup_flag = UP_WAKEUP;

        P2IFG &= ~UP_BTN_MASK; // clears interrupt flag


    } else if ((P2IFG & DN_BTN_MASK) == DN_BTN_MASK) {
        wakeup_flag = DN_WAKEUP;

        P2IFG &= ~DN_BTN_MASK;


    } else if ((P2IFG & SL_BTN_MASK) == SL_BTN_MASK) {
        wakeup_flag = SL_WAKEUP;

        P2IFG &= ~SL_BTN_MASK;


    } else if ((P2IFG & BK_BTN_MASK) == BK_BTN_MASK) {
        wakeup_flag = BK_WAKEUP;

        P2IFG &= ~BK_BTN_MASK;
    }

    LPM0_EXIT;
}

