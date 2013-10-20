# BlackRamElctronics.com

CC=gcc

OUTPUT = oled_demo

LIBS =
LIBS_DIR =
INCLUDES= -IDrivers/OLED_MONO/

CFLAGS= -Wall $(INCLUDES)
CSOURCES= oled_demo.c Drivers/OLED_MONO/OLED_Driver.c Drivers/OLED_MONO/OLED_HWIF.c
CPPSOURCES= 

all: $(OUTPUT)
        
$(OUTPUT): $(CSOURCES)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

clean:
	rm $(OUTPUT)
        
