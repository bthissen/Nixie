# Nixie

This is the control program for a Nixie Clock using an ATMEGA 328p uC coupled with a DS3231M RTC to maintain and read-back the time
to IN-12B Nixie Tubes and signal with INS-1 Neon indicators (for AM/PM and colon blink). Code is in C and pre-compiled with necessary
.hex to flash directly if needed (need USBASP programmer with ICSP protocol).

Also incluced are KiCAD schematic and PCB files for editing if desired and compiled gerber files for PCB manufacured if wanted. (note: circuit
uses a CD74HC4514 4-to-16 mux to drive the nixie tubes.

#v1.0 initial publish of working model and PCB (currently not using twi.c or twi.h; plan to incorporate for ease of use for TWI protocol
