#include <msp430.h> 
#include "motor.h"
#include "display.h"

/**
 * motor.c
 */

volatile int wdt_cycle_counter = 0;
volatile unsigned long wdt_timer_counter = 0;

unsigned int timer_day, timer_hour, timer_min, timer_sec;

volatile int timer_behavior = -1;

void motor_setup() {
    __bis_SR_register(GIE);
    DISABLE_WDT;

    BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO

    TA0CTL = TASSEL_1 + MC_1;     // ACLK, upmode
    TA0CCR0 = 240;              // Interrupt should happen every ~12 kHz / 240 = 50Hz
    TA0CCR1 = 0;
    TA0CCTL1 = OUTMOD_7; // enable PWM

    P2DIR |= BIT6;                         // Make P2.1, 2.5 output pins (Piezo)
    P2OUT = ~BIT6;                      // set to 0 output (ground)
    P2SEL |= BIT6;                                // enable PWM for P2.1
    P2SEL &= ~BIT7;
    P2SEL2 &= ~(BIT6 | BIT7);
}

void set_motor_angle(int angle_deg){
     wdt_cycle_counter =0;
     int ccr = (int) ((0.1 * (float) (angle_deg)) + 6.0);
     P2SEL |= BIT6;
     TA0CCR1 = ccr;
     ENABLE_WDT;
}

void turn_on_light(){
    set_motor_angle(ON_ANGLE);
}

void turn_off_light(){
    set_motor_angle(OFF_ANGLE);
}

void prank_light(){
    int iterations, delay_cycle_counter;
    for (iterations = 0; iterations < 15; iterations++){
        if (iterations % 2 == 0){
            turn_off_light();
        } else {
            turn_on_light();
        }

        unsigned int delay = TA0R;


        __delay_cycles(1100000);

        delay_cycle_counter = 0;

        while (delay_cycle_counter < delay){
            __delay_cycles(27500);

            delay_cycle_counter++;
        }
    }


}


void set_timer_mode(int mode){
    timer_behavior = mode;
}

int get_timer_mode(){
    return timer_behavior;
}

void set_timer(int days, int hours, int min, int sec){
    wdt_timer_counter = (unsigned long) (1.5*(86400 * days + 3600 * hours + 60 * min + sec));
}

void update_timer(){
    unsigned long timer_count = wdt_timer_counter / 1.5;

    timer_day = timer_count / 86400L;
    timer_count -= ((unsigned long) timer_day) * (86400L);

    timer_hour = timer_count / 3600L;
    timer_count -= ((unsigned long) timer_hour) * (3600L);

    timer_min = timer_count / 60L;
    timer_count -= ((unsigned long) timer_min) * (60L);

    timer_sec = timer_count;
}

unsigned int get_timer_day(){
    return timer_day;
}

unsigned int get_timer_hour(){
    return timer_hour;
}

unsigned int get_timer_min(){
    return timer_min;
}

unsigned int get_timer_sec(){
    return timer_sec;
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
    if (wdt_cycle_counter == 1) {
        TA0CCR1 = 0;
    }

    if (wdt_cycle_counter == 10){
        set_display_color(OFF);
    }

    if (wdt_cycle_counter < 20){
        wdt_cycle_counter++;
    }

    if (state == 14){
        if(wdt_timer_counter % 3 == 0){
            LPM0_EXIT;
        }
    }

    if (wdt_timer_counter > 0){

        wdt_timer_counter--;
    }

    if (wdt_timer_counter == 1) {
        switch(timer_behavior) {
            case 0:
                turn_on_light();
                break;

            case 1:
                turn_off_light();
                break;

            case 2:
                state = 13;
                LPM0_EXIT;
                break;
        }

    }
}

