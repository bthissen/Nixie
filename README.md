# Nixie

This is the control program for a Nixie Clock using an ATMEGA 328p uC coupled with a DS3231M RTC to maintain and read-back the time
to IN-12B Nixie Tubes and signal with INS-1 Neon indicators (for AM/PM and colon blink). Code is in C and pre-compiled with necessary
.hex to flash directly if needed (need USBASP programmer with ICSP protocol).

Also incluced are KiCAD schematic and PCB files for editing if desired and compiled gerber files for PCB manufacured if wanted. (note: circuit
uses a CD74HC4514 4-to-16 mux to drive the nixie tubes.

#v1.0 initial publish of working model and PCB (currently not using twi.c or twi.h; plan to incorporate for ease of use for TWI protocol
#v1.5 incorporated twi.c and twi.h files for (I2C) implementation (and future modularity); also added 12hr/24hr toggle mode for UI

USAGE:
clock display using 4 nixie tubes arranged as hour(10's) | hour(1's) | minute(10's) | minute(1's) with blinking ':' indicator and 'PM' neon light represent
time over 12 hours (12hr mode only).

Time Setting:
two buttons (set and increment) are used to allow for user program of time and mode. If set button not pressed, increment button acts as toggle mode 
which will switch display from 12 hour to 24 hour and vice-versa.

to set time, press set button once (colon indicator will freeze) and select minutes with increment button to cycle minutes from 0 to 59. Once minutes is decided 
press set button again to increment hours using same increment button (1 to 12 in 12 hr mode; 0 to 24 in 24hr mode). Once hours is selected press set button for 
final time to program time. If done correctly displayed time will be written to DS3231 RTC and colon indicator will begin to blink again.
