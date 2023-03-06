/********************************************
*	Bradley Thissen
*	07/25/2018
*	
*	main file for implementing TWI (I2C)
*	for AVR architecture (mainly atmega328p)
*********************************************/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"


//initialize TWI (I2C)
void twi_init() {
	TWSR=0x00;	//set prescale bits to zero (1)
	TWBR=0x48;	//[Fclk / (2*TWBR*prescale + 16) ] = SCL	(16MHz, TWBR=72) SCL ~ 100kHz
	TWCR=0x04;	//enable TWU register	(1<<TWEN)
}

//Start TWI Communication
void twi_start() {
	TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
	while (!(TWCR & (1<<TWINT)));		//loop until complete
}

//End TWI communication
void twi_stop(void) {
	TWCR = ((1<<TWINT) | (1<<TWEN) | (1<<TWSTO));
	_delay_us(100);	//delay a short time for cleanup
}

//Output data over TWI (I2C)
void twi_write(uint8_t v_twidata){
	TWDR = v_twidata;					//set data into buffer register
	TWCR = ((1<<TWINT) | (1<<TWEN));	//reset interrupt flag and  enable
	while (!(TWCR & (1<<TWINT)));		//loop until complete
}

//Read data from TWI (I2C)
uint8_t twi_read(uint8_t v_ackoption) {
	TWCR = ((1<<TWINT) | (1<<TWEN) | (v_ackoption<<TWEA));	//reset flag, enable, set acknowledge bit
	while ( !(TWCR & (1 <<TWINT)));							//loop until complete
    return TWDR;
}