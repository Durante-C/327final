/*
 * motor.h
 * CHRISTIAN DURANTE, RYAN PAI, NICO ROSSI, SAM XU
 * FINAL PROJECT
 * ELEC 327
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define DISABLE_WDT WDTCTL = WDTPW | WDTHOLD; \
        IE1 &= ~WDTIE

#define ENABLE_WDT WDTCTL = WDT_ADLY_250; \
        IE1 |= WDTIE

#define ON_ANGLE 115
#define OFF_ANGLE 65

extern volatile int state;

void motor_setup();
void set_motor_angle(int angle_deg);

void turn_on_light();
void turn_off_light();


#endif /* MOTOR_H_ */
