/*
 * serial.h
 * CHRISTIAN DURANTE, RYAN PAI, NICO ROSSI, SAM XU
 * FINAL PROJECT
 * ELEC 327
 */

#ifndef SERIAL_H_
#define SERIAL_H_



void flash_spi_detected();
void spi_setup();
char * byte2decimal(int byte);
char * ip_concat(char *ipstring, char *str1, char *str2, char *str3, char *str4);



#endif /* MOTOR_H_ */
