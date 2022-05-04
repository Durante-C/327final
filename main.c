#include "msp430g2553.h"

#include "display.h"
#include "motor.h"
#include "button.h"
#include "serial.h"

// used to check if SEL button was used to wake up msp
#define IF_SEL if(wakeup_flag == SL_WAKEUP)

// this macro is to be used instead of break statement in case for brevity
#define ENDCASE WAIT_DISPLAY;\
                LPM0; \
                break

// does mod-10 arithmetic for timer display
#define TIMER_VAL_10S_1S timer_val_10s = timer_val / 10; \
                         timer_val_1s = timer_val - (timer_val_10s * 10)

#define TIMER_VAL_10S (char) ('0' + timer_val_10s)
#define TIMER_VAL_1S (char) ('0' + timer_val_1s)

char * ipaddress; // THIS VARIABLE STORES THE IP ADDRESS
const char ip_address_str[12] = "IP ADDRESS:\r";
char ip_length;   // Recieved from serial.c to properly trim displayed IP address

const char newline[1] = "\r";

// a whole bunch of strings to be printed to display
const char introline1[14] = "FINAL PROJECT\r";
const char introline2[8] = "ELEC 327";

const char turnonlight[13] = "Turn On Light";
const char turnofflight[14] = "Turn Off Light";

const char prankmode[10] = "Prank Mode";
const char smiley[2] = ":)";

const char authorsline1[] = "C.Durante,R.Pai\r";
const char authorsline2[] = "N. Rossi, S. Xu";

const char setcustomtimer[] = "Set Custom Timer";
const char controllightswitch[] = "Control Light\rSwitch";
const char checktimer[11] = "Check Timer";

const char timer[7] = "Timer: ";
char timer_disp[15] = "00d 00h 00m 00s";

const char aftertimerends[] = "After Timer Ends:\r";

const char settimer[] = "Set Timer\r";
const char sec10[] = "10sec"; // 5
const char sec20[] = "20sec"; // 5
const char sec30[] = "30sec"; // 5
const char min1[] = "1min";
const char min5[] = "5min";
const char min10[] = "10min";
const char min15[] = "15min";
const char min20[] = "20min";
const char min30[] = "30min";
const char min45[] = "45min";
const char hr1[] = "1hr";
const char hr2[] = "2hr";
const char hr3[] = "3hr";
const char hr4[] = "4hr";
const char hr5[] = "5hr";
const char hr6[] = "6hr";
const char hr7[] = "7hr";
const char hr8[] = "8hr";
const char hr9[] = "9hr";
const char hr10[] = "10hr";
const char hr11[] = "11hr";
const char hr12[] = "12hr";
const char hr13[] = "13hr";
const char hr14[] = "14hr";
const char hr15[] = "15hr";
const char hr16[] = "16hr";
const char d1[] = "1d";
const char d2[] = "2d";
const char d3[] = "3d";

volatile int wakeup_flag = 0;
volatile int menu_sel = 0;
volatile int timer_mode = 0;
volatile int timer_setting = 0;

unsigned int timer_val = 0;
char timer_val_10s = 0;
char timer_val_1s = 0;

volatile int state = 0;
int prev_state = 0;

void main(void)
{

    ENABLE_WDT; //Necessary for command timer

    motor_setup();
    set_motor_angle(90); //Serves as the "neutral" position of the motor, returned to at end of commands.
    switch_setup();
    spi_setup();

    initDisplay();  // Initialize the display

    clearDisplay(); // Clear the display. Important if the display has not been power cycled since last run

    WAIT_DISPLAY; // Inefficient way to wait for the end of writing

    set_display_color(WHITE);   //To ensure consistent back-lighting

    WAIT_DISPLAY;

    while(1){
        wakeup_flag = 0;
        clearDisplay();
        WAIT_DISPLAY;               //Note, must be used between writing each line to ensure no corruption
        set_display_color(WHITE);
        WAIT_DISPLAY;

        switch(state){
            case 0:
                // WAKEUP MODE.
                prev_state = 0;

                printBytes(introline1, 14);
                WAIT_DISPLAY;
                printBytes(introline2, sizeof(introline2));
                WAIT_DISPLAY;

                LPM0;
                clearDisplay();
                WAIT_DISPLAY;
                printBytes(authorsline1, sizeof(authorsline1) - 1);
                WAIT_DISPLAY;
                printBytes(authorsline2, sizeof(authorsline2) - 1);
                WAIT_DISPLAY;

                LPM0;

                state = 1;

                break;

            case 1:
                //Main menu, can scroll through any functionality
                prev_state = 0;

                menu_sel = modulo_check(menu_sel, 3); //Allows for continuous "endless" scrolling through options

                switch(menu_sel) {                    //Each case similar, displays different options when moved onto, can move on to actual command modes
                    case 0:
                        printBytes(setcustomtimer, sizeof(setcustomtimer) - 1);
                        WAIT_DISPLAY;
                        LPM0;

                        IF_SEL
                            state = 11;

                        break;

                    case 1:
                        printBytes(controllightswitch, sizeof(controllightswitch) - 1);
                        WAIT_DISPLAY;
                        LPM0;
                        IF_SEL
                            state = 2;
                        break;

                    case 2:
                        printBytes(checktimer, 11);
                        WAIT_DISPLAY;
                        LPM0;
                        IF_SEL
                            state = 14;
                        break;

                    case 3:
                        printBytes(ip_address_str, 12);
                        WAIT_DISPLAY;
                        ipaddress = ip_export();
                        ip_length = len_export();
                        printBytes(ipaddress, ip_length);
                        WAIT_DISPLAY;
                        LPM0;
                        break;

                } // switch (state 1)
                break;

            case 2:
                // BASIC ON/OFF FUNCTIONALITY
                prev_state = 1;

                menu_sel = modulo_check(menu_sel, 1);

                switch(menu_sel){
                    case 0:
                        printBytes(turnonlight, sizeof turnonlight);
                        WAIT_DISPLAY;
                        LPM0;

                        if (wakeup_flag == SL_WAKEUP) {
                            turn_on_light();
                        }

                        break;

                    case 1:
                        printBytes(turnofflight, sizeof turnofflight);
                        WAIT_DISPLAY;
                        LPM0;

                        if (wakeup_flag == SL_WAKEUP) {
                            turn_off_light();
                        }

                        break;



                    default:
                        menu_sel = 0;
                } // switch

                break;

            case 11:
                //SETTING A TIMER
                prev_state = 1;
                menu_sel = modulo_check(menu_sel, 27);

                printBytes(settimer, sizeof settimer - 1);
                WAIT_DISPLAY;

                switch(menu_sel) {  //Need many options of timer lengths to display
                    case 0:
                        printBytes(sec10, 5);
                        ENDCASE;

                    case 1:
                        printBytes(sec20, 5);
                        ENDCASE;

                    case 2:
                        printBytes(sec30, 5);
                        ENDCASE;

                    case 3:
                        printBytes(min1, 4);
                        ENDCASE;

                    case 4:
                        printBytes(min5, 4);
                        ENDCASE;

                    case 5:
                        printBytes(min10, 5);
                        ENDCASE;

                    case 6:
                        printBytes(min20, 5);
                        ENDCASE;

                    case 7:
                        printBytes(min30, 5);
                        ENDCASE;

                    case 8:
                        printBytes(min45, 5);
                        ENDCASE;

                    case 9:
                        printBytes(hr1, 3);
                        ENDCASE;

                    case 10:
                        printBytes(hr2, 3);
                        ENDCASE;

                    case 11:
                        printBytes(hr3, 3);
                        ENDCASE;

                    case 12:
                        printBytes(hr4, 3);
                        ENDCASE;

                    case 13:
                        printBytes(hr5, 3);
                        ENDCASE;

                    case 14:
                        printBytes(hr6, 3);
                        ENDCASE;

                    case 15:
                        printBytes(hr7, 3);
                        ENDCASE;

                    case 16:
                        printBytes(hr8, 3);
                        ENDCASE;

                    case 17:
                        printBytes(hr9, 3);
                        ENDCASE;

                    case 18:
                        printBytes(hr10, 4);
                        ENDCASE;

                    case 19:
                        printBytes(hr11, 4);
                        ENDCASE;

                    case 20:
                        printBytes(hr12, 4);
                        ENDCASE;

                    case 21:
                        printBytes(hr13, 4);
                        ENDCASE;

                    case 22:
                        printBytes(hr14, 4);
                        ENDCASE;

                    case 23:
                        printBytes(hr15, 4);
                        ENDCASE;

                    case 24:
                        printBytes(hr16, 4);
                        ENDCASE;

                    case 25:
                        printBytes(d1, 2);
                        ENDCASE;

                    case 26:
                        printBytes(d2, 2);
                        ENDCASE;

                    case 27:
                        printBytes(d3, 2);
                        ENDCASE;

                }
                //Carries over selection from this state to a subsequent states to choose action
                IF_SEL{
                    timer_setting = menu_sel;
                    menu_sel = 0;
                    state = 12;
                }

                break;

            case 12: // CHOOSE ON OR OFF OR PRANK FOR TIMER
                prev_state = 11;
                menu_sel = modulo_check(menu_sel, 2);

                printBytes(aftertimerends, sizeof(aftertimerends) - 1);
                WAIT_DISPLAY;

                switch(menu_sel) {
                    case 0:
                        printBytes(turnonlight, 13);
                        break;

                    case 1:
                        printBytes(turnofflight, 14);
                        break;

                    case 2:
                        printBytes(prankmode, 10);
                        break;


                }


                WAIT_DISPLAY;
                LPM0;

                IF_SEL {
                    set_timer_mode(menu_sel);

                    switch(timer_setting){
                        case 0: // 10s
                            set_timer(0, 0, 0, 10);
                            break;

                        case 1: // 20s
                            set_timer(0, 0, 0, 20);
                            break;

                        case 2: // 30s
                            set_timer(0,0,0,30);
                            break;

                        case 3: // 1min
                            set_timer(0,0,1,0);
                            break;

                        case 4: // 5min
                            set_timer(0,0,5,0);
                            break;

                        case 5: // 10min
                            set_timer(0,0,10,0);
                            break;

                        case 6: // 20min
                            set_timer(0,0,20,0);
                            break;

                        case 7: // 30min
                            set_timer(0,0,30,0);
                            break;

                        case 8: // 45min
                            set_timer(0,0,40,0);
                            break;

                        case 9: // 1hr
                            set_timer(0,1,0,0);
                            break;

                        case 10: // 2hr
                            set_timer(0,2,0,0);
                            break;

                        case 11: // 3hr
                            set_timer(0,3,0,0);
                            break;

                        case 12: // 4hr
                            set_timer(0,4,0,0);
                            break;

                        case 13: // 5hr
                            set_timer(0,5,0,0);
                            break;

                        case 14: // 6hr
                            set_timer(0,6,0,0);
                            break;

                        case 15: // 7hr
                            set_timer(0,7,0,0);
                            break;

                        case 16: // 8hr
                            set_timer(0,8,0,0);
                            break;

                        case 17: // 9hr
                            set_timer(0,9,0,0);
                            break;

                        case 18: // 10hr
                            set_timer(0,10,0,0);
                            break;

                        case 19: // 11hr
                            set_timer(0,11,0,0);
                            break;

                        case 20: // 12hr
                            set_timer(0,12,0,0);
                            break;

                        case 21: // 13hr
                            set_timer(0,13,0,0);
                            break;

                        case 22: // 14hr
                            set_timer(0,14,0,0);
                            break;

                        case 23: // 15hr
                            set_timer(0,15,0,0);
                            break;

                        case 24: // 16hr
                            set_timer(0,16,0,0);
                            break;

                        case 25: // 1d
                            set_timer(1,0,0,0);
                            break;

                        case 26: // 2d
                            set_timer(2,0,0,0);
                            break;

                        case 27: // 3d
                            set_timer(3,0,0,0);
                            break;

                        default:
                            set_timer(0,0,0,1);
                            break;
                    } //switch

                    state = 1;

                }// IF_SEL


                break;

            case 13: // PRANK mode
                prev_state = 0;

                set_display_color(RED);
                WAIT_DISPLAY;

                prank_light();

                state = 0;

                break;

            case 14: // CHECK TIMER mode
                prev_state = 1;

                printBytes(timer, 7);
                WAIT_DISPLAY;
                timer_mode = get_timer_mode();
                switch(timer_mode){
                    case 0:
                        printBytes(turnonlight+8, 5); // Light
                        WAIT_DISPLAY;
                        printBytes(turnonlight+4, 3); // On
                        WAIT_DISPLAY;
                        break;

                    case 1:
                        printBytes(turnonlight+8, 5); // Light
                        WAIT_DISPLAY;
                        printBytes(turnofflight+4, 4); // Off
                        break;

                    case 2:
                        printBytes(smiley, 2); // Smiley face (shh don't let them know :)
                        break;
                }
                WAIT_DISPLAY;

                printBytes(newline, 1);
                WAIT_DISPLAY;

                update_timer();

                // "XXd XXh XXm XXs"

                // DAY
                timer_val = get_timer_day();
                TIMER_VAL_10S_1S;
                timer_disp[0] = TIMER_VAL_10S; // performs modulo calculations inside the macro and converts to char
                timer_disp[1] = TIMER_VAL_1S;

                // HOUR
                timer_val = get_timer_hour();
                TIMER_VAL_10S_1S;
                timer_disp[4] = TIMER_VAL_10S;
                timer_disp[5] = TIMER_VAL_1S;

                // MIN
                timer_val = get_timer_min();
                TIMER_VAL_10S_1S;
                timer_disp[8] = TIMER_VAL_10S;
                timer_disp[9] = TIMER_VAL_1S;

                // SEC
                timer_val = get_timer_sec();
                TIMER_VAL_10S_1S;
                timer_disp[12] = TIMER_VAL_10S;
                timer_disp[13] = TIMER_VAL_1S;

                printBytes(timer_disp, 15);
                WAIT_DISPLAY;

                LPM0;
                break;



        } // switch

        button_handler();

    } // while

} // main

int modulo_check(int num, int max){
    if (num < 0){
        num = max;
    }
    else if (num > max){
        num = 0;
    }

    return num;
}

void button_handler(){
    // This function handles "Up" "Down" and "Back" buttons (whose functionality is common between modes).
    // "Sel" button is handled separately.
    switch(wakeup_flag){
        case UP_WAKEUP:
            menu_sel++;
            break;

        case DN_WAKEUP:
            menu_sel--;
            break;

        case BK_WAKEUP:
            state = prev_state;
            menu_sel = 0;
            break;
    }
}



