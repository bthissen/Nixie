/**************************************************************************
 * Bradley Thissen
 * 
 * Nixie Clock Control Software (v1.0)
 * 
 * writes/reads clock info from RTC (DS3231M)
 * drives IN-12B (x4) Nixie Tubes with neon indicators for display
 * includes every hour tube cycle for cathode poisoning mitigation
 * 
 * 3/04/2023
***************************************************************************/
#define F_CPU 16000000UL		//set uC clk to 16MHz

#include <avr/io.h>				//enable AVR definitions/Macros
#include <stdlib.h>				//C std lib for basic coding functionality
#include <util/delay.h>			//embedded libraries and delay/timings
#include <string.h>				//c String libraries
#include <stdbool.h>            //bool (binary algebra libraries)

#define RTC_read 0b11010001		//TWI DS3231 adress (1101000) W/R set to 1
#define RTC_write 0b11010000	//TWI DS3231 adress (1101000) W/R set to 0

struct rtc_data{

    uint8_t min;        //hold complete min byte from RTC
    uint8_t hour;       //hold complete hour byte from RTC
    
    uint8_t min0;       //hold only lower nibble (0's min)
    uint8_t min1;       //hold only upper nibble (1's min)
    uint8_t hour0;      //hold only lower nibble (0's hour)
    uint8_t hour1;      //hold only upper nibble (1's hour)

    bool PM;            //store AM/PM bit
    bool twelve_hr;     //store 12hr/24hr bit
};

void nix_num(int nix_sel, int number){  //display given number to selected tube

    DDRD = 0xFF;                        //set port D to all outputs
    uint8_t sel;                        //to add to PORTD for correct tube output

    if (nix_sel == 1){sel = 0xF1;}      //select mintues digit '0'
    else if (nix_sel == 2){sel = 0xF2;} //select hours digit '1'
    else if (nix_sel == 3){sel = 0xF4;} //select minutes digit '0'
    else if (nix_sel == 4){sel = 0xF8;} //select hours digit '1'sel = 0xF8;}
    else {sel = 0xFF;}                  //select all digits on

    _delay_us(5800); //decrease for less flicker but more ghosting (increase for brighter digits) <--------
    
    switch(number){
        case 0 :
            PORTD = (0x0F & sel);       
            break;
        case 1 :
            PORTD = (0x1F & sel);      
            break;
        case 2 :
            PORTD = (0x2F & sel);    
            break;
        case 3 :
            PORTD = (0x3F & sel);  
            break;
        case 4 :
            PORTD = (0x4F & sel);   
            break;
        case 5 :
            PORTD = (0x5F & sel);  
            break;
        case 6 :
            PORTD = (0x6F & sel);  
            break;
        case 7 :
            PORTD = (0x7F & sel);    
            break;
        case 8 :
            PORTD = (0x8F & sel);   
            break;
        case 9 :
            PORTD = (0x9F & sel);  
            break;
        default :
            DDRB = 0x03;                //enable outputs for neons
            PORTB = 0x02;               //turn on the PM indicator (error state number)
            break;
    }
}

void cycle_nix(){

    //int digit = 0;        //remove comment and add loop to increment each digit separately
    int val = 0;

    //for(digit = 1; digit < 5; digit++){       //iterate which tube to display
        for (val = 0; val < 10; val++){         //iterate each number
            nix_num(0,val);                     //display to nixie tube
            _delay_ms(250);                     //150ms delay
        }
    //}
    PORTD = 0x00;                               //no digit selected if out of loop (cycle complete)
}

/***********************BASE TWI (I2C) Functions*********************************/
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
uint8_t twi_read(unsigned int ack) {
	TWCR = ((1<<TWINT) | (1<<TWEN) | (ack<<TWEA));			//reset flag, enable, set acknowledge bit
	while ( !(TWCR & (1 <<TWINT)));							//loop until complete
    return TWDR;
}
/***********************BASE TWI (I2C) Functions*********************************/

/*************************DS3231 initialization**********************************/
void clk_init(void){

	twi_init();				//setup TWI
	//twi_start();			//start sequence for TWI

	//twi_write(RTC_write);	//send DS3231 address with write enable
	//twi_write(0x0E);		//select DS3231 addr pointer to control register
	//twi_write(0x1C);		//update control reg with default values (of DS3231 datasheet)
	//twi_stop();		    //stop TWI (need to set ACK to 0 for RTC???)
}

void RTC_set(struct rtc_data set){

	twi_start();
	twi_write(RTC_write);	//upload and send RTC TWI bus address
	twi_write(0x01);		//set RTC pointer reg to minutes (0x00) start

	//begin RTC update (only first three for now to check basic functionality)
	twi_write(set.min);
	twi_write(set.hour);

	twi_stop();				//stop TWI (need to set ACK to 0 for RTC???)
}

struct rtc_data RTC_get(struct rtc_data get){

	twi_start();			//need to init first??? <-------------
	twi_write(RTC_write); 	//select write to RTC to adjust reg pointer
	twi_write(0x01);		//set pointer address of RTC back to 1 or minutes register
	twi_stop();				//stop TWI

	twi_start();			//need to init first??? <-------------
	twi_write(RTC_read);	//set back to read mode to grab the data from RTC

	get.min = twi_read(1);	//read minutes, input into get and send ack to continue
	get.hour = twi_read(0);	//read hours, mask all but the BCD value [(0x1F & twi_read(0))]

	twi_stop();				//stop TWI

	return get;
}
/*************************DS3231 initialization**********************************/

int8_t button_set() { //in theory, this should only run if button is pressed
  static uint16_t state = 0; //holds present state
  state = (state << 1) | (! bit_is_clear(PINC, 2)) | 0xE000;
  if (state == 0xF000) return 1;
  return 0;
}//debounce_switch

int8_t button_inc() { //in theory, this should only run if button is pressed
  static uint16_t state = 0; //holds present state
  state = (state << 1) | (! bit_is_clear(PINC, 3)) | 0xE000;
  if (state == 0xF000) return 1;
  return 0;
}//debounce_switch

int main(){

    DDRD = 0xFF;                         //set outputs for the nixie tubes
    DDRB = 0x03;                         //enable output for colon and PM neons

    DDRC = ((0<< PB2) | (0 << PB3));     //set DDRC pins 2 and 3 as inputs (for buttons)
    PORTC = ((1 << PB2) | (1 << PB3));   //set those same pins as pull ups (logic set as low)

    uint8_t timer = 0x00;                //incrementer for delayed I2C data gather
    bool colon = 1;
    int timeset = 0;

    struct rtc_data set;
    //time to load before bootup for testing only
    //set.min = 0x58;                      //load minutes byte (0x58 == 58 minutes)
    //set.hour = 0x71;                     //load hours byte (set to 11 hours || bit 5 PM indicate)

    clk_init();		                     //initialize DS3231 for operation
	//RTC_set(set);                      //update RTC with default time (if needed)

    cycle_nix();                         //cycle Nixie Tubes on startup

    struct rtc_data clock;
    clock = RTC_get(clock);              //read the current time

    while(1){

        /************************UI Programming**************************************/
        if(button_set() == 1){           //detect button press for set
            timeset++;
            if(timeset == 3){            //if done with setting time reset and program RTC
                timeset = 0;
                set.hour = clock.hour;   //update set struct with current clock input and program
                set.min = clock.min;
                RTC_set(set);
            }
        }

        if((button_inc() == 1) && (timeset != 0)){
            
            switch(timeset){
                case 1 :                                    //increment minutes
                    clock.min0++;
                    if (clock.min0 > 9){
                        clock.min0 = 0x00;
                        clock.min1++;
                        if(clock.min1 > 0x05){clock.min1 = 0x00;}
                    }
                    clock.min = ((clock.min1 << 4) | (clock.min0));
                    break;

                case 2 :                                   //increment hours and return 12hr setting
                    //clock.hour = (0x1F & clock.hour);        
                    clock.hour0++;
                    if (clock.hour0 > 0x09){
                        clock.hour0 = 0x00;
                        clock.hour1 = 0x01;
                    }

                    if((clock.hour1 == 0x01) && (clock.hour0 > 0x02)){
                        clock.hour1 = 0;
                        clock.hour0 = 1;
                    }
                    
                    if((clock.hour1 == 1) && (clock.hour0 == 2)){
                        clock.PM = !clock.PM;              //toggle AM or PM
                    }
                    clock.hour = ((clock.hour1 << 4) | (clock.hour0));
                    clock.hour = ((0x01 << 6) | (clock.PM << 5) | (clock.hour));
                    break;

                default :
                    //anything to add here?
                    break;
            }
        }  
        /************************UI Programming**************************************/

        if ((timer == 0x38) && (timeset == 0)){
            clock = RTC_get(clock); //read the current time
            colon = !colon;         //toggle colon
            timer = 0x00;

            if (clock.min == 0){    //start of the hour
            cycle_nix();            //cycle Nixie Tubes prevent cathode poisoning
            }
        }

        //feed time into set values for nixie diplay
        clock.min0 = (0x0F & clock.min);
        clock.min1 = ((0xF0 & clock.min) >> 4);

        clock.hour0 = (0x0F & clock.hour);
        clock.hour1 = ((0xF0 & clock.hour) >> 4);
        clock.hour1 = (clock.hour1 & 0x03);

        if(clock.hour1 == 0x02){
            clock.PM = 1;
            clock.hour1 = 0x00;
        }
        else if(clock.hour1 == 0x03){
            clock.PM = 1;
            clock.hour1 = 0x01;
        }

        else if((clock.hour1 == 0x00) || (clock.hour1 == 0x01)){
            clock.PM = 0;
        }

        PORTB = ((colon << PB0) | (clock.PM << PB1));

        //display values to nixie tube
        nix_num(1, clock.min0);
        nix_num(2, clock.min1);
        nix_num(3, clock.hour0);
        if(clock.hour1 != 0){
            nix_num(4, clock.hour1);
        }
        timer++;
    }
    return 0;
}
