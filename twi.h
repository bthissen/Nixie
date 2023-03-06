/********************************************
*	Bradley Thissen
*	07/25/2018
*	
*	header file for implementing TWI (I2C)
*	for AVR architecture (mainly atmega328p)
*********************************************/
#ifndef _TWI_H
#define _TWI_H

#include <avr/io.h>
#include <stdlib.h>

void twi_init();						//initialize prototype
void twi_start();						//start prototype
void twi_stop(void);					//end prototype
void twi_write(uint8_t v_twidata);		//write data prototype
uint8_t twi_read(uint8_t v_ackoption);		//read acknowledge prototype

#endif
