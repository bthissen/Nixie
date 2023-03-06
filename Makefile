PRG            =nixieclk

OBJS            = $(PRG).o

SRCS		   =nixieclk.c

MCU_TARGET     = atmega328p
OPTIMIZE       = -O2    # options are 1, 2, 3, s
CC             = avr-gcc
F_CPU          = 16000000UL

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf text eeprom #lst was here after .elf
	cat $(PRG).hex

$(PRG).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS) -DF_CPU=$(F_CPU)

clean: 
	rm -rf $(PRG).o $(PRG).elf $(PRG).bin $(PRG).hex $(PRG).srec 
	rm -rf $(PRG)_eeprom.bin $(PRG)_eeprom.hex $(PRG)_eeprom.srec
#	rm -rf *.lst *.map 

#setup for for USB programmer
#may need to be changed depending on your programmer
program: $(PRG).hex
	avrdude -c usbasp -p m328p -e -U flash:w:$(PRG).hex  -v

# lst:  $(PRG).lst

# %.lst: %.elf
#	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

#include the dependencies from the other makefiles
#%.d: %.c
#	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

#-include $(SRCS:.c=.d)

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@
