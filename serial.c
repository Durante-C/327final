//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, SPI 3-Wire Slave Data Echo
//
//   Description: SPI slave talks to SPI master using 3-wire mode. Data received
//   from master is echoed back.  USCI RX ISR is used to handle communication,
//   CPU normally in LPM4.  Prior to initial data exchange, master pulses
//   slaves RST for complete reset.
//   ACLK = n/a, MCLK = SMCLK = DCO ~1.2MHz
//
//   Use with SPI Master Incremented Data code example.  If the slave is in
//   debug mode, the reset signal from the master will conflict with slave's
//   JTAG; to work around, use IAR's "Release JTAG on Go" on slave device.  If
//   breakpoints are set in slave RX ISR, master must stopped also to avoid
//   over-running slave RXBUF.
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          | |             XOUT|-
// Master---+-|RST              |
//            |             P1.2|<- Data Out (UCB0SOMI)
//            |                 |
//            |             P1.1|-> Data In (UCB0SIMO)
//            |                 |
//            |             P1.4|<- Serial Clock In (UCB0CLK)
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include "msp430g2553.h"
#include <string.h>

#define DISABLE_WDT WDTCTL = WDTPW | WDTHOLD; \
        IE1 &= ~WDTIE

#define ENABLE_WDT WDTCTL = WDT_ADLY_1000; \
        IE1 |= WDTIE

#define nop()  __no_operation()

char cmdbuf[20];
char cmd_index=0;

/** Delay function. **/
void delay(unsigned int d) {
  int i;
  for (i = 0; i<d; i++) {
    nop();
  }
}

void flash_spi_detected(void) {
    int i=0;
    P1OUT = 0;
//    for (i=0; i < 5; ++i) {
//        set_motor_angle(i * 30);
//        delay(0x4fff);
//        delay(0x4fff);
//    }
}

void spi_setup(void)
{
  __bis_SR_register(GIE);
  DISABLE_WDT;

  BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO

  /*
   * Redundant initializations from motor setup (?)
  TA0CTL = TASSEL_1 + MC_1;     // ACLK, upmode
  TA0CCR0 = 240;              // Interrupt should happen every ~12 kHz / 240 = 50Hz
  TA0CCR1 = 0;
  TA0CCTL1 = OUTMOD_7; // enable PWM

  P2DIR |= BIT6;                         // Make P2.1, 2.5 output pins (Piezo)
  P2OUT = ~BIT6;                      // set to 0 output (ground)
  P2SEL |= BIT6;                                // enable PWM for P2.1
  P2SEL &= ~BIT7;
  P2SEL2 &= ~(BIT6 | BIT7);
  */

  P1SEL = BIT6 + BIT7 + BIT5;
  P1SEL2 = BIT6 + BIT7 + BIT5;
  UCB0CTL1 = UCSWRST;                       // **Put state machine in reset**
  UCB0CTL0 |= UCMSB + UCSYNC + UCCKPH;      // UCCKPH 3-pin, 8-bit SPI master
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt
}

char * byte2decimal(int byte){
    unsigned int hundreds, tens, ones;
    char str[3];

    hundreds = byte / 100;
    byte -= hundreds * 100;

    tens = byte / 10;
    byte -= tens * 10;

    ones = byte;

    str[0] = (char) '0' + hundreds;
    str[1] = (char) '0' + tens;
    str[2] = (char) '0' + ones;

    return str;
}

char * ip_concat(char *ipstring, char *str1, char *str2, char *str3, char *str4){
    strncpy(ipstring, str1, 3);
    strncpy(ipstring+4, str2, 3);
    strncpy(ipstring+8, str3, 3);
    strncpy(ipstring+12, str4, 3);

    return ipstring;
}

__attribute__((interrupt(USCIAB0RX_VECTOR))) void USCI0RX_ISR (void)
{
    /*
  if (UCB0STAT & UCOE) {
    P1OUT |= BIT0;
  }
  */
  char value = UCB0RXBUF;
  volatile char temp[20];
  volatile int cmd_test = cmd_index;
  int i;
  for (i = 0; i < 20; i++) {
      temp[i] = cmdbuf[i];
  }
  /*
  if (value == '\n') {
    if (strncmp(cmdbuf, "HELLO WORLD", 11) == 0) {
      set_motor_angle(180);
      LPM0;
      set_motor_angle(90);
      LPM0;
      set_motor_angle(0);
    } else {
      set_motor_angle(0);
    }
    cmd_index = 45;
  */
  if (value == 0x00) {
      set_motor_angle(0);
  }
  if (value == 0x01) {
      set_motor_angle(180);
    }

  /*
  } else {
    cmdbuf[cmd_index] = value;
    cmd_index++;
  }
  */

}


