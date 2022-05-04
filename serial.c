/*
 * This implementation was adapted from a tutorial (that itself is a use of a tutorial provided by TI) that can be found here:
 * https://mitchtech.net/raspberry-pi-msp430-spi/
 *
 * The linked code (or at least this adaptation of its) was from 2012 and outdated in a number of locations that we been remedied
 * Some additional functions involving our original method of interpreting the ip address are left in.
 *
 * The largest alteration is in the interrupt where the data from the pi is processed.
 */

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
char sticky_len=0;
char message[257];
int ip_check = 0;
char mess_len = 0;
char mess_po = 0;


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

}

void spi_setup(void)
{
  __bis_SR_register(GIE);
  DISABLE_WDT;

  BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO


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

//int j = 0;

char * ip_export(void){
    return message;
    }

char len_export(void){
    return sticky_len;
    }

__attribute__((interrupt(USCIAB0RX_VECTOR))) void USCI0RX_ISR (void)
{
    /*
    Code within this interrupt is responsible for the MSP-end of the serial communication, receiving and storing data or executing commands from the pi
  */

  char value = UCB0RXBUF;

  if (value != 0x00) {                  //Constantly reading any non-null characters received
      cmdbuf[cmd_index] = value;        //Storing as they come, one after another to avoid timing issues, detailed in report
      cmd_index++;
  }


  if (value == 0x01) {                  //Signal that a message should be interpereted, a 1, comes at the END
      if (cmdbuf[0] == 2) {             //Look back at the first character to interpret command type as...
          turn_off_light();             //turning the light on
      }
      else if (cmdbuf[0] == 3) {
          turn_on_light();              //turning the light off
      }
      else if (cmdbuf[0] == 5) {
          prank_light();                //executing prank mode, repeatedly turning code on and off
      }
      else if (cmdbuf[0] == 4) {        //Receiving ip address of raspberry pi, then...
          sticky_len = cmdbuf[1];       //The next bit is the length of the message, pre-determined after ip generation on pi-end since it can vary
          for (mess_po = 2; mess_po < (cmdbuf[1] + 2); mess_po++) {     //As long as we haven't scanned that many bytes...
              message[(mess_po - 2)] = cmdbuf[mess_po];                 //Write those bytes to the message to display. Indices adjusted due to first two 'category' bytes.
          }
      }
      cmd_index = 0;                    //After we're done receiving a message (after any 1), reset index so next message acan check code
  }

}


